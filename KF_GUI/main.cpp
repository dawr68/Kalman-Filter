#include "KF_GUI.h"
#include <QtWidgets/QApplication>
#include "FilterManager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    KF_GUI mainWindow;
    FilterManager kalman_filter;

    mainWindow.setFilterManager(&kalman_filter);
    mainWindow.setupGraphs();
    mainWindow.show();

    return app.exec();
}