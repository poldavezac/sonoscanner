#pragma once
#include "dataslice.h"
#include "data.h"

#include <chrono>
#include <mutex>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtCharts/QChartGlobal>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QLineEdit>
#include <QBoxLayout>
#include <QPushButton>
#include "gui.h"

namespace sc {
  // all information relative to the GUI configuration
  struct Theme
  {
    size_t      width  = 400;
    size_t      height = 400;
    std::string title  = "Simple line chart example";
    std::string starttitle = "Start";
    std::string stoptitle  = "Stop";
    std::string quittitle  = "Quit";
    std::string normalizationrangetitle  = "Slinding Window";
  };

  // Gui data that needs to change during app runtime
  struct GuiLiveData
  {
    Model         model;
    Data          data;
    DataSlice     slice = {};
    std::mutex    mutex = {};
  };

  // TODO: understand why Qt example is using *naked* pointers
  // I expect that Qt handles their destruction.
  struct Gui {
    QApplication  &app;
    QLineSeries * series;
    QChart      * chart;
    QChartView  * view;
    QBoxLayout  * layout;
    QLineEdit   * ymin;
    QLineEdit   * ymax;

    Theme             theme;
    GuiLiveData       live;

    Gui(QApplication &, Model const &, Data const &);
    void runchartthread();  // in update_chart.cpp
  };


  // start the gui
  void run(QApplication &, Model &, Data &);
}
