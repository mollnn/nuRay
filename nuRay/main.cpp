#include "widget.h"

#include <QApplication>
#include "matggxrefl.h"
#include "utils.h"

int main(int argc, char *argv[])
{
    // Test energy conservation
    Utils utils;
    utils.testEnergyConversion();

    QApplication a(argc, argv);
    Widget w;
    w.show();

    return a.exec();
}
