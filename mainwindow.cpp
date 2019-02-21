#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>

#include "clothingwindow.h"
#include "weaponwindow.h"
#include "tattoowindow.h"
#include "utility.h"

void MainWindow::openClothing() {
    // Open an existing clothing mod
    statusBar()->clearMessage();
    statusBar()->showMessage("Opening clothing item...");
    auto *cw = new ClothingWindow(true, this);
    cw->show();
}

void MainWindow::newClothing() {
    // Create a new clothing mod (using default data)
    statusBar()->clearMessage();
    statusBar()->showMessage("Creating clothing item...");
    auto *cw = new ClothingWindow(false, this);
    cw->show();
}

void MainWindow::openTattoo() {
    // Open an existing tattoo mod
    statusBar()->clearMessage();
    statusBar()->showMessage("Opening tattoo...");
    auto *tw = new TattooWindow(true, this);
    tw->show();
}

void MainWindow::newTattoo() {
    // Create a new tattoo mod (using default data)
    statusBar()->clearMessage();
    statusBar()->showMessage("Creating tattoo...");
    auto *tw = new TattooWindow(false, this);
    tw->show();
}

void MainWindow::openWeapon() {
    // Open an existing weapon mod
    statusBar()->clearMessage();
    statusBar()->showMessage("Opening weapon...");
    auto *ww = new WeaponWindow(true, this);
    ww->show();
}

void MainWindow::newWeapon() {
    // Create a new weapon mod (using default data)
    statusBar()->clearMessage();
    statusBar()->showMessage("Creating weapon...");
    auto *ww = new WeaponWindow(false, this);
    ww->show();
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    // Setup
    ui->setupUi(this);
    setAttribute(Qt::WA_QuitOnClose);
    setFixedSize(MainWindow::size());
    statusBar()->showMessage("Do NOT close this window or it will close all windows.");


    // Connect buttons for clothing widget
    connect(ui->openClothingButton, SIGNAL(pressed()), SLOT(openClothing()));
    connect(ui->newClothingButton, SIGNAL(released()), SLOT(newClothing()));

    // Connect buttons for tattoo widget
    connect(ui->openTattooButton, SIGNAL(pressed()), SLOT(openTattoo()));
    connect(ui->newTattooButton, SIGNAL(pressed()), SLOT(newTattoo()));

    // Connect buttons for weapon widget
    connect(ui->openWeaponButton, SIGNAL(pressed()), SLOT(openWeapon()));
    connect(ui->newWeaponButton, SIGNAL(pressed()), SLOT(newWeapon()));


    // Initialize internal resources
    Q_INIT_RESOURCE(resources);


    // Setup QDom
    QDomImplementation::setInvalidDataPolicy(QDomImplementation::DropInvalidChars);


    // Initial setup for widgets
    QString err = QString();

    // Enchantment widget check
    if (!EnchantmentWidget::load_effects())
        err.append("Failed to load enchantment data!\n");

    // Tags widget check
    if (!TagsWidget::load_tags())
        err.append("Failed to load tags data!\n");

    // Colours widget check
    if (!ColoursWidget::load_colours())
        err.append("Failed to load colours data!\n");

    // If we can't load editor data, it's considered fatal
    if (!err.isEmpty()) {
        Utility::error(err);
        this->close();
        return;
    }

    // Clothing defs check
    if (!ClothingWindow::load_defs()) {
        ui->openClothingButton->setEnabled(false);
        ui->newClothingButton->setEnabled(false);
    }

    // Weapon defs check
    if (!WeaponWindow::load_defs()) {
        ui->openWeaponButton->setEnabled(false);
        ui->newWeaponButton->setEnabled(false);
    }

    // Tattoo defs check
    if (!TattooWindow::load_defs()) {
        ui->openTattooButton->setEnabled(false);
        ui->newTattooButton->setEnabled(false);
    }
}

MainWindow::~MainWindow()
{
    // Ui
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // Overrides the close event. Pops up a dialog warning and either ignores the event or accepts it (which will close ALL windows) depending on user input.
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Warning", "This will close all windows.\nAre you sure you want to quit?", QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        event->accept();
        QApplication::exit();
    } else {
        event->ignore();
    }
}
