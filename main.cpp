#include <QApplication>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    // Standard auto-gen main function. Does nothing special.

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return QApplication::exec();
}
