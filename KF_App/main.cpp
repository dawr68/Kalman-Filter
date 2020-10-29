#include "KF_App.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    KF_App w;
    w.show();
    return a.exec();
}
