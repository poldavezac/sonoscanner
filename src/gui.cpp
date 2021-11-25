#include <thread>
#include <chrono>
#include <mutex>
#include <QBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QRegularExpressionValidator>
#include "gui.h"

QT_USE_NAMESPACE
namespace sc {
  namespace {
    template<typename T>
    void _set_window(T * view, Theme const & theme)
    {
      QMainWindow window;
      auto widget = new QWidget();
      window.setCentralWidget(widget);
      widget->setLayout(view);
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

    QLineEdit * _add_normrange(Gui & gui)
    {
      auto value = std::to_string(gui.live.model.slidingrange);
      auto btn = new QLineEdit(value.data());
      btn->setValidator(new QRegularExpressionValidator(
        QRegularExpression("[0-9]*.{0,1}[0-9]*"), btn
      ));
      QObject::connect(btn, &QLineEdit::editingFinished, [&gui, &btn](){
          auto val = gui.live.model.slidingrange;

          auto txt = btn->text().toStdString();
          try{ val = std::stof(txt); }
          catch(...) { return; }

          std::lock_guard<std::mutex> _(gui.live.mutex);
          gui.live.model.slidingrange = val;
          gui.live.data.normalize(val);
      });
      return btn;
    }

    QBoxLayout * _add_buttons(Gui & gui)
    {
      auto layout = new QBoxLayout(QBoxLayout::LeftToRight);
      layout->addWidget(_add_button<kRunning>(gui.theme.starttitle, gui, [](){}));
      layout->addWidget(_add_button<kStopped>(gui.theme.stoptitle,  gui, [](){}));
      layout->addWidget(new QLabel(gui.theme.slidingrangetitle.data()));
      layout->addWidget(_add_normrange(gui));
      layout->addWidget(_add_button<kDisabled>(gui.theme.quittitle,  gui, [&gui](){ gui.app.exit(); }));
      return layout;
    }
  }

  Gui::Gui(QApplication & app, Model const & mdl, Data const & dat):
    app(app),
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
    this->live.model.state = kStopped;
  }

  void run(QApplication & app, Model & model, Data & data)
  {
    qDebug("Starting GUI");
    Gui gui(app, model, data);
    qDebug("Started GUI, starting stream");

    std::thread thr([&gui]{ gui.runchartthread(); }); 
    qDebug("Started stream");
    app.exec();
  }
}
