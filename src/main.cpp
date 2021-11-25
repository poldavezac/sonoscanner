#include "gui.h"
#include <QtGlobal>
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    std::string path = argc <= 1 ? std::string("") : std::string(argv[1]);
    qInfo("Reading file '%s'", path.data());
    auto model = sc::Model{path};
    auto data  = sc::open(model);
    qInfo("Found %lu lines", data.raw.size());

    QApplication app(argc, argv);
    qDebug("Started app");
    sc::run(app, model, data);
}
