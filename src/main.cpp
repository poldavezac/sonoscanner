#include <iostream>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <QtGlobal>
#include <QtWidgets/QApplication>
#include "gui.h"

int main(int argc, char *argv[])
{
  // Declare the supported options.
  namespace po = boost::program_options;
  po::options_description desc("Allowed options");
  desc.add_options()  // Watch out! boost::program_options has a strange way of adding options!
      ("help", "Viewer for ECG data")
      ("file", po::value<std::string>(), "data file path, default \"../../data/ecg.txt\"")
      ("window", po::value<float>(), "window range (s)")
      ("normalization", po::value<float>(), "normalization window range (s)")
  ;

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count("help")) {
    std::cout << desc << "\n";
    return 1;
  }

  std::string path;
  if (vm.count("file"))
    path = vm["file"].as<std::string>();
  else {
    // select the default path as <root>/data/ecg.txt
    path = "../../data/ecg.txt";
    if(!boost::filesystem::exists(path))
      path = "../data/ecg.txt";
    if(!boost::filesystem::exists(path))
      path = "data/ecg.txt";
  }

  qInfo("Reading file '%s'", path.data());
  auto model = sc::Model{path};

  if (vm.count("normalization"))
    model.normalizationrange = std::max(0.f, vm["normalization"].as<float>());
  if (vm.count("window"))
    model.windowrange = std::max(0.f, vm["window"].as<float>());

  auto data  = sc::open(model);
  qInfo("Found %lu lines", data.raw.size());

  QApplication app(argc, argv);
  qDebug("Started app");
  sc::run(app, model, data);
  return 1;
}
