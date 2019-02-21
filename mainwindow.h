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
public slots:
    // Simple slot functions
    void openClothing();
    void newClothing();

    void openTattoo();
    void newTattoo();

    void openWeapon();
    void newWeapon();

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    // Override closeEvent to intercept things like pressing the 'x' button.
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::MainWindow *ui; // Ui
};

#endif // MAINWINDOW_H
