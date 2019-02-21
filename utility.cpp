#include "utility.h"

#include <QMessageBox>

void Utility::error(const QString& error)
{
    // Just opens a dialog box displaying the given string.

    QMessageBox box;
    box.setText(error);
    box.exec();
}
