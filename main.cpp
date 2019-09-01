#include <QApplication>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    // Standard main function. Does nothing special.

    // The idea here is to eventually move all the libraries to that folder. This works on two... out of the nine.
    QApplication::addLibraryPath("./dependencies");

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return QApplication::exec();
}
