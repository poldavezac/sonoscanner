#include <thread>
#include <chrono>
#include <mutex>
#include <QBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QRegularExpressionValidator>
#include "gui.h"

QT_USE_NAMESPACE
namespace sc {
  namespace {
    template <DisplayState S, typename T>
    QPushButton * _add_button(std::string const & title, Gui & gui, T && other)
    {
      auto btn = new QPushButton(title.data());
      QObject::connect(btn, &QPushButton::clicked, [&gui, other](){
          std::lock_guard<std::mutex> _(gui.live.mutex);
          gui.live.model.state = S; 
          other();
      });
      return btn;
    }

    template<typename T>
    QLineEdit * _add_lineedit(Gui & gui, T && func)
    {
      auto value = std::to_string(gui.live.model.normalizationrange);
      auto btn = new QLineEdit(value.data());
      btn->setValidator(new QRegularExpressionValidator(
        QRegularExpression("[0-9]*.{0,1},{0,1}[0-9]*"), btn
      ));
      QObject::connect(btn, &QLineEdit::editingFinished, [&gui, btn, func](){
          auto val = 0.f;

          auto txt = btn->text().toStdString();
          try{ val = std::stof(txt); }
          catch(...) {
            qDebug("Failed to parse value: '%s'", txt.data());
            return; 
          }
          std::lock_guard<std::mutex> _(gui.live.mutex);
          func(val);
      });
      return btn;
    }

    QLineEdit * _add_normrange(Gui & gui)
    {
      return _add_lineedit(gui, [&gui](float val) {
          val = std::max(0.f, val);
          qDebug("Changing normalization range to %f", val);
          gui.live.model.normalizationrange = val;
          gui.live.data.normalize(val);
          gui.live.model.state = kRefresh;
      });
    }

    QBoxLayout * _add_buttons(Gui & gui)
    {
      auto layout = new QBoxLayout(QBoxLayout::LeftToRight);
      layout->addWidget(_add_button<kRunning>(
        gui.theme.starttitle, gui, [&gui](){ gui.live.slice = {}; }
      ));
      layout->addWidget(_add_button<kStopped>(gui.theme.stoptitle,  gui, [](){}));
      layout->addWidget(new QLabel(gui.theme.normalizationrangetitle.data()));
      layout->addWidget(_add_normrange(gui));
      layout->addWidget(_add_button<kDisabled>(
        gui.theme.quittitle,  gui, [&gui](){ gui.app.exit(); }
      ));
      return layout;
    }

    QBoxLayout * _add_yaxisedit(Gui & gui)
    {
      auto layout = new QBoxLayout(QBoxLayout::LeftToRight);
      auto btn = new QPushButton("Automatic Y Range");
      QObject::connect(btn, &QPushButton::clicked, [&gui](){
          std::lock_guard<std::mutex> _(gui.live.mutex);
          gui.live.model.zoom = gui.live.model.zoom == kAuto ? kFixed : kAuto;
      });
      gui.ymin = _add_lineedit(gui, [&gui](float val) {
          qDebug("Parsed value ymin: '%f'", val);
          gui.live.model.zoom = kFixed;
          gui.live.model.state = kRefresh;
      });
      gui.ymax = _add_lineedit(gui, [&gui](float val) {
          qDebug("Parsed value ymax: '%f'", val);
          gui.live.model.zoom = kFixed;
          gui.live.model.state = kRefresh;
      });
      layout->addWidget(btn);
      layout->addWidget(new QLabel("Y min / max"));
      layout->addWidget(gui.ymin);
      layout->addWidget(gui.ymax);
      return layout;
    }
  }

  Gui::Gui(QApplication & app, Model const & mdl, Data const & dat):
    app(app),
    series(new QLineSeries()),
    chart(new QChart()),
    view(new QChartView(this->chart)),
    layout(new QBoxLayout(QBoxLayout::TopToBottom)),
    live{mdl, dat}
  {
    this->chart->legend()->hide();
    this->chart->addSeries(this->series);
    this->chart->createDefaultAxes();
    this->chart->setTitle(this->theme.title.data());
    this->view->setRenderHint(QPainter::Antialiasing);

    layout->addWidget(this->view);
    layout->addLayout(_add_buttons(*this));
    layout->addLayout(_add_yaxisedit(*this));
    this->live.model.state = kStopped;
  }

  void run(QApplication & app, Model & model, Data & data)
  {
    qDebug("Starting GUI");
    Gui gui(app, model, data);

    qDebug("Creating main window");
    QMainWindow window;
    auto widget = new QWidget();
    window.setCentralWidget(widget);
    widget->setLayout(gui.layout);
    window.resize(gui.theme.width, gui.theme.height);
    window.show();

    gui.live.model.state = kRefresh;

    qDebug("starting stream");
    std::thread thr([&gui]{ gui.runchartthread(); }); 

    qDebug("Executing app");
    app.exec();
  }
}
