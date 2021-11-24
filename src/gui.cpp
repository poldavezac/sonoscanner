#include <thread>
#include <chrono>
#include <mutex>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtCharts/QChartGlobal>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QBoxLayout>
#include <QPushButton>
#include "gui.h"

QT_USE_NAMESPACE
namespace sc {
  namespace {
    struct Gui {
      QLineSeries * const series;
      QChart      * const chart;
      QChartView  * const view;
      DataSlice     slice = {};
      std::mutex    mutex = {};

    };

    Gui _add_chart(Theme const & theme)
    {
      // TODO: understand why Qt example is using *naked* pointers
      QLineSeries *series = new QLineSeries();

      QChart *chart = new QChart();
      chart->legend()->hide();
      chart->addSeries(series);
      chart->createDefaultAxes();
      chart->setTitle(theme.title.data());

      QChartView *view = new QChartView(chart);
      view->setRenderHint(QPainter::Antialiasing);

      return {series, chart, view};
    }

    template<typename T>
    void _set_window(T * view, Theme const & theme)
    {
      QMainWindow window;
      window.setLayout(view);
      window.resize(theme.width, theme.height);
      window.show();
    }

    template <DisplayState S, typename T>
    QPushButton * _add_button(std::string const & title, Gui & gui, Model & model, T && other)
    {
      auto btn = new QPushButton(title.data());
      QObject::connect(btn, &QPushButton::clicked, [&gui, &model, other](){
          {
            std::lock_guard<std::mutex> _(gui.mutex);
            model.state = S; 
          }
          other();
      });
      return btn;
    }

    QBoxLayout * _add_buttons(
        QApplication & app, Gui & gui, Theme const & theme, Model & model, Data & data
    )
    {
      auto layout = new QBoxLayout(QBoxLayout::LeftToRight);
      layout->addWidget(_add_button<kRunning>(theme.starttitle, gui, model, [](){}));
      layout->addWidget(_add_button<kStopped>(theme.stoptitle,  gui, model, [](){}));
      layout->addWidget(_add_button<kDisabled>(theme.quittitle, gui, model, [&app](){ app.exit(); }));
      return layout;
    }

  }

  // gui interactions
  namespace {
    bool _update_chart(Gui & gui, Model const & model, Data const & data)
    {
      auto next = gui.slice.refresh(model, data);
      if(next == gui.slice)
        return false;

      if(
          (gui.slice.begin == gui.slice.end)
          || (gui.slice.begin > next.begin)
          || (gui.slice.end > next.end)
      ) {
        // refresh completely
        gui.series->clear();
        for(auto i = next.begin; i < next.end; ++i)
          gui.series->append(data.normed[i].time, data.normed[i].value);
      }
      else
      {
        // update points
        gui.series->removePoints(0, next.begin - gui.slice.begin);
        for(auto i = gui.slice.end; i < next.end; ++i)
          gui.series->append(data.normed[i].time, data.normed[i].value);
      }
      gui.slice = next;
      return true;
    }

    template <Qt::Orientation> qreal _axis_min(Gui const & gui);
    template <Qt::Orientation> qreal _axis_max(Gui const & gui);
    template <> qreal _axis_min<Qt::Horizontal>(Gui const & gui)
    { return gui.series->points().front().rx(); }
    template <> qreal _axis_max<Qt::Horizontal>(Gui const & gui)
    { return gui.series->points().back().rx(); }
    template <> qreal _axis_min<Qt::Vertical>(Gui const & gui)
    {
      auto val = std::numeric_limits<qreal>::max();
      for(auto pt: gui.series->points())
        val = std::min(val, pt.ry());
      return val;
    }
    template <> qreal _axis_max<Qt::Vertical>(Gui const & gui)
    {
      auto val = std::numeric_limits<qreal>::min();
      for(auto pt: gui.series->points())
        val = std::max(val, pt.ry());
      return val;
    }

    template <Qt::Orientation O>
    inline void _update_axis(Gui const & gui, Model const & model)
    {
      // expecting only one axis
      assert(gui.chart->axes(O).size() == 1lu);
      auto axis = gui.chart->axes(O).front();
      if(gui.series->points().size() == 0lu)
      {
        axis->setMin(0.f);
        axis->setMax(1.f);
      } else {
        auto front = _axis_min<O>(gui);
        auto back  = _axis_max<O>(gui);
        axis->setMin(front - (back-front) * model.axispadding);
        axis->setMax(back  + (back-front) * model.axispadding);
      }
    }

    void _run_thread(Gui & gui, Model const & model, Data const & data)
    {
      while(model.state != kDisabled)
      {
        if(gui.mutex.try_lock())
        {
          try{
            if(model.state == kRunning && _update_chart(gui, model, data))
            {
              _update_axis<Qt::Horizontal>(gui, model);
              if(model.zoom == kAuto)
                _update_axis<Qt::Vertical>(gui, model);
              gui.chart->update();
            }
          }
          catch (...){
            gui.mutex.unlock();
            std::throw_with_nested(std::runtime_error("Failed update"));
          }
        }
        std::this_thread::sleep_for(
            std::chrono::milliseconds((int) std::round(1000.f/model.refreshrate))
        );
      }
    }
  }

  int run(int argc, char **argv, Model & model, Data & data)
  {
    QApplication app(argc, argv);
    Theme theme;
    auto gui    = _add_chart(theme);
    auto layout = new QBoxLayout(QBoxLayout::TopToBottom);
    layout->addWidget(gui.view);
    layout->addLayout(_add_buttons(app, gui, theme, model, data));
    _set_window(layout, theme);

    std::thread thr([&gui, &model, &data]{ _run_thread(gui, model, data); }); 
    return app.exec();
  }
}
