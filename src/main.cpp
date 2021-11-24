#include "gui.h"
#include <QtGlobal>

int main(int argc, char *argv[])
{
    std::string path = argc <= 2 ? std::string("") : std::string(argv[1]);
    qInfo("Reading file '%s'", path.c_str());
    auto model = sc::Model{path};
    auto data  = sc::open(model);
    return sc::run(argc, argv, model, data);
}
