#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

// Main window. Shows basically the main menu of the application.
// This is the parent window and takes responsibility for all child widgets, so closing this will close ALL windows.

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    enum Mod { // These are used as array indexes. Don't break them.
        clothing, tattoo, weapon,
        somebody_fucked_up // Also used as a size variable
    };

public slots:
    // Simple slot functions
    void openMod();
    void newMod();

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    // Override closeEvent to intercept things like pressing the 'x' button.
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::MainWindow *ui; // Ui
    QVector<bool> enabled; // List of enabled widgets. In case somebody fucked up a def file and the widget fails to load.

    Mod get_mode();
};

#endif // MAINWINDOW_H
