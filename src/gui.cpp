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
    template<typename T>
    void _set_window(T * view, Theme const & theme)
    {
      QMainWindow window;
      window.setLayout(view);
      window.resize(theme.width, theme.height);
      window.show();
    }

    template <DisplayState S, typename T>
    QPushButton * _add_button(std::string const & title, Gui & gui, T && other)
    {
      auto btn = new QPushButton(title.data());
      QObject::connect(btn, &QPushButton::clicked, [&gui, other](){
          {
            std::lock_guard<std::mutex> _(gui.live.mutex);
            gui.live.model.state = S; 
          }
          other();
      });
      return btn;
    }

    QBoxLayout * _add_buttons(Gui & gui)
    {
      auto layout = new QBoxLayout(QBoxLayout::LeftToRight);
      layout->addWidget(_add_button<kRunning>(gui.theme.starttitle, gui, [](){}));
      layout->addWidget(_add_button<kStopped>(gui.theme.stoptitle,  gui, [](){}));
      layout->addWidget(_add_button<kDisabled>(gui.theme.quittitle,  gui, [&gui](){ gui.app.exit(); }));
      return layout;
    }
  }

  Gui::Gui(int argc, char **argv, Model const & mdl, Data const & dat):
    app(argc, argv),
    series(new QLineSeries()),
    chart(new QChart()),
    view(new QChartView(this->chart)),
    live{mdl, dat}
  {
    this->chart->legend()->hide();
    this->chart->addSeries(this->series);
    this->chart->createDefaultAxes();
    this->chart->setTitle(this->theme.title.data());
    this->view->setRenderHint(QPainter::Antialiasing);

    auto layout = new QBoxLayout(QBoxLayout::TopToBottom);
    layout->addWidget(this->view);
    layout->addLayout(_add_buttons(*this));
    _set_window(layout, this->theme);
  }

  int run(int argc, char **argv, Model & model, Data & data)
  {
    Gui gui(argc, argv, model, data);

    std::thread thr([&gui]{ gui.runchartthread(); }); 
    return gui.app.exec();
  }
}
