#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QFileDialog>

#include "clothingwindow.h"
#include "weaponwindow.h"
#include "tattoowindow.h"
//#include "outfitwindow.h" // DISABLED FOR BUILD
#include "utility.h"

void MainWindow::openMod()
{
    // Load a mod file. Opens a file dialogue and once a file is selected, tries to detect mod type and handles it accordingly.

    // Open file dialogue and get path of selected file.
    QString location = QFileDialog::getOpenFileName(this, "Open existing mod", "./", "(*.xml)");

    // Should trigger on cancel or failure
    if (location.isNull() || location.isEmpty())
        return;

    // Create our file object.
    QFile file(location);

    // If you manage to trigger this, please tell me how the flying fuck you did that.
    if (!file.exists()) {
        Utility::error("File not found.");
        return;
    }

    // There are a few things that could cause us to not be able to open the file.
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        Utility::error("Could not open file!");
        return;
    }

    // We need to create a document and make sure we can parse it.
    QDomDocument doc(file.fileName());
    QString err = QString();
    if (!doc.setContent(&file, &err)) {
        Utility::error("Could not parse xml file!\n" + err);
        file.close();
        return;
    }
    file.close(); // We don't need this anymore.

    // Now we just need to check the mod type and create the mod object as the mod class itself will handle the rest.

    if (doc.documentElement().tagName() == "clothing") { // Is clothing mod?

        // Check if window was disabled.
        if (!enabled[MainWindow::Mod::clothing]) {
            Utility::error("The clothing mod tool has been disabled because something went wrong.");
            return;
        }

        // Update statuse message.
        statusBar()->clearMessage();
        statusBar()->showMessage("Opened a clothing mod.");

        // Create window object and show it.
        auto *cw = new ClothingWindow(doc, location, this);
        cw->show();
    }
    else if (doc.documentElement().tagName() == "tattoo") { // Is tattoo mod?

        // Check if window was disabled.
        if (!enabled[MainWindow::Mod::tattoo]) {
            Utility::error("The tattoo mod tool has been disabled because something went wrong.");
            return;
        }

        // Update statuse message.
        statusBar()->clearMessage();
        statusBar()->showMessage("Opened a tattoo mod.");

        // Create window object and show it.
        auto *tw = new TattooWindow(doc, location, this);
        tw->show();
    }
    else if (doc.documentElement().tagName() == "weapon") { // Is weapon mod?

        // Check if window was disabled.
        if (!enabled[MainWindow::Mod::weapon]) {
            Utility::error("The weapon mod tool has been disabled because something went wrong.");
            return;
        }

        // Update statuse message.
        statusBar()->clearMessage();
        statusBar()->showMessage("Opened a weapon mod.");

        // Create window object and show it.
        auto *ww = new WeaponWindow(doc, location, this);
        ww->show();
    }
    /*
    else if (doc.documentElement().tagName() == "outfit") { // Is outfit mod?
        // TODO
    }
    */
    else Utility::error("Not a valid mod file!"); // Bad. I only accept mod xml files.
}

void MainWindow::newMod()
{
    // Create a new mod. Utilizes default files to open a new editor for selected mod type.

    // Our file object
    QFile file;

    // Mod flag
    MainWindow::Mod flag = MainWindow::Mod::somebody_fucked_up;

    // Determine which default file we will be loading.
    switch (get_mode()) {
        case MainWindow::Mod::clothing: {
            file.setFileName(":/res/data_files/clothing_default.xml");
            flag = MainWindow::Mod::clothing;
            break;
        }
        case MainWindow::Mod::tattoo: {
            file.setFileName(":/res/data_files/tattoo_default.xml");
            flag = MainWindow::Mod::tattoo;
            break;
        }
        case MainWindow::Mod::weapon: {
            file.setFileName(":/res/data_files/weapon_default.xml");
            flag = MainWindow::Mod::weapon;
            break;
        }
        /*
        case MainWindow::Mod::outfit: {
            file.setFileName(":/res/data_files/outfit_default.xml");
            flag = MainWindow::Mod::outfit;
            break;
        }
        */
        case MainWindow::Mod::somebody_fucked_up:
            Utility::error("Critical problem: Somebody doesn't know what the fuck they're doing.");
            return;
    }

    // An impressive feat if you trip this.
    if (!file.exists()) {
        Utility::error("Failed to create file!");
        return;
    }

    // This... shouldn't ever trigger... Right?
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        Utility::error("Could not open default file!");
        return;
    }

    // We need to create a document and make sure we can parse it.
    QDomDocument doc(file.fileName());
    QString err = QString();
    if (!doc.setContent(&file, &err)) {
        Utility::error("Could not parse xml file!\n" + err);
        file.close();
        return;
    }
    file.close(); // We don't need this anymore.

    // Yet another switch to create the window object
    switch (flag) {
        case MainWindow::Mod::clothing: {
            // Check if window was disabled.
            if (!enabled[MainWindow::Mod::clothing]) {
                Utility::error("The clothing mod tool has been disabled because something went wrong.");
                return;
            }

            // Update statuse message.
            statusBar()->clearMessage();
            statusBar()->showMessage("Created a clothing mod.");

            // Create window object and show it.
            auto *cw = new ClothingWindow(doc, QString(), this);
            cw->show();
            break;
        }
        case MainWindow::Mod::tattoo: {
            // Check if window was disabled.
            if (!enabled[MainWindow::Mod::tattoo]) {
                Utility::error("The tattoo mod tool has been disabled because something went wrong.");
                return;
            }

            // Update statuse message.
            statusBar()->clearMessage();
            statusBar()->showMessage("Created a tattoo mod.");

            // Create window object and show it.
            auto *tw = new TattooWindow(doc, QString(), this);
            tw->show();
            break;
        }
        case MainWindow::Mod::weapon: {
            // Check if window was disabled.
            if (!enabled[MainWindow::Mod::weapon]) {
                Utility::error("The weapon mod tool has been disabled because something went wrong.");
                return;
            }

            // Update statuse message.
            statusBar()->clearMessage();
            statusBar()->showMessage("Created a weapon mod.");

            // Create window object and show it.
            auto *ww = new WeaponWindow(doc, QString(), this);
            ww->show();
            break;
        }
        /*
        case MainWindow::Mod::outfit: {
        // Check if window was disabled.
            if (!enabled[MainWindow::Mod::outfit]) {
                Utility::error("The outfit mod tool has been disabled because something went wrong.");
                return;
            }

            // Update statuse message.
            statusBar()->clearMessage();
            statusBar()->showMessage("Created an outfit mod.");

            // Create window object and show it.
            auto *ow = new OutfitWindow(doc, QString(), this);
            ow->show();
            break;
        }
        */
        case MainWindow::Mod::somebody_fucked_up:
            Utility::error("Seriously?"); // It would be against everything I know to be real if this was in any way reachable.
    }
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    // Setup
    ui->setupUi(this);
    setAttribute(Qt::WA_QuitOnClose);
    setFixedSize(MainWindow::size());
    statusBar()->showMessage("Do NOT close this window or it will close all windows.");


    // Connections
    connect(ui->newButton, SIGNAL(released()), SLOT(newMod()));
    connect(ui->loadButton, SIGNAL(released()), SLOT(openMod()));


    // Initialize internal resources
    Q_INIT_RESOURCE(resources);


    // Setup QDom
    QDomImplementation::setInvalidDataPolicy(QDomImplementation::DropInvalidChars);


    // Some vars
    QString err = QString();
    enabled = QVector<bool>(MainWindow::Mod::somebody_fucked_up, true); // Creates a vector with a size equal to the count of values in MainWindow::Mod.

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
    if (!ClothingWindow::load_defs())
        enabled[MainWindow::Mod::clothing] = false;

    // Weapon defs check
    if (!WeaponWindow::load_defs())
        enabled[MainWindow::Mod::weapon] = false;

    // Tattoo defs check
    if (!TattooWindow::load_defs())
        enabled[MainWindow::Mod::tattoo] = false;

    // Outfit defs check
    // TODO
}

MainWindow::~MainWindow()
{
    delete ui; // Ui
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

MainWindow::Mod MainWindow::get_mode()
{
    // Returns enum value related to the current mod type selected in the modComboBox

    // Current text of combo box.
    QString text = ui->modComboBox->currentText();

    // Clothing mod
    if (text == "Clothing mod")
        return MainWindow::Mod::clothing;

    // Tattoo mod
    if (text == "Tattoo mod")
        return MainWindow::Mod::tattoo;

    // Weapon mod
    if (text == "Weapon mod")
        return MainWindow::Mod::weapon;

    /*
    // Outfit mod
    if (text == "Outfit mod")
        return MainWindow::Mod::outfit;
    */

    return MainWindow::Mod::somebody_fucked_up; // Seriously.
}
