#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName("FBI");
    a.setApplicationName("SvgDemo");

    MainWindow w;
    w.show();

    return a.exec();
}
