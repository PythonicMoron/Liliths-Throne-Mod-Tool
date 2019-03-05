#include "colourswidget.h"
#include "ui_colourswidget.h"

#include <QShortcut>
#include <QWhatsThis>
#include <QCloseEvent>
#include <QJsonDocument>
#include <QJsonArray>

#include "utility.h"

ColoursWidget::ColoursWidget(QWidget *parent) : QWidget(parent), ui(new Ui::ColoursWidget), showing_colours(false)
{
    // Setup
    ui->setupUi(this);
    setWindowFlag(Qt::Window);
    hide();

    // Shortcuts
    new QShortcut(QKeySequence("F1"), this, SLOT(whats_this()));

    // Initialize colour pointer
    colour_ptr = nullptr;

    // Connect
    connect(ui->doneButton, &QPushButton::released, [this]() {apply();});
}

ColoursWidget::~ColoursWidget()
{
    delete ui;
}

void ColoursWidget::open(Colour *target, bool is_preset)
{
    // Sets the colour pointer to passed pointer, sets up the widget, hides the parent window, and displays the widget.

    // Set current colour pointer
    colour_ptr = target;

    // Determine what data to display or if any change is necessary
    if (ui->listWidget->count() < colours.count() && ui->listWidget->count() < presets.count())
        if (is_preset)
            show_presets();
        else
            show_colours();
    else
        if (is_preset) {
            if (showing_colours)
                show_presets();
        } else
            if (!showing_colours)
                show_colours();

    ui->listWidget->clearSelection();

    // Select item(s) in widget
    if (is_preset) {// Preset check

        QString selected = colour_ptr->getPreset(); // Will return empty if not a string. This is ok here
        for (int i = 0; i < presets.size(); i++)
            if (selected == presets[i]) {
                ui->listWidget->item(i)->setSelected(true);
                break;
            }

    } else { // If preset check is false then we're dealing with a colour list

        QStringList selected = colour_ptr->getColours(); // Will return empty if not a list. This is ok here
        for (const QString& col : selected)
            for (int i = 0; i < colours.size(); i++)
                if (col == colours[i]) {
                    ui->listWidget->item(i)->setSelected(true);
                    break;
                }
    }

    // Display widget
    parentWidget()->hide();
    show();
}

void ColoursWidget::reload_ui()
{
    // This only really hides the widget if it's open and shows the parent window.

    // Show parent (if needed) and hide widget
    if (!parentWidget()->isVisible())
        parentWidget()->show();
    hide();
}

bool ColoursWidget::load_colours(bool force_internal)
{
    // This function loads the data from the colours json file and replaces the static ui data.

    // Returns succuss state (only returns false if both internal and external files failed to load).
    // The force_internal parameter tells the function to ignore external files, if any, when set to true. Defaults to false.

    QFile file("colours.json"); // First, check for external file. Allows for overriding internal resource.
    if (!file.exists() || force_internal) {
        file.setFileName(":/res/data_files/colours.json"); // If no external file exists or this was called with force_internal set to true, load internal file.
        if (!file.exists()) {
            Utility::error("Could not open a valid colours.json file!"); // Something or someone really fucked up.
            return false;
        }
    }

    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString data = file.readAll();
    file.close();
    QJsonDocument json_colours = QJsonDocument::fromJson(data.toUtf8());

    // The internal file is compiled in and unable to be edited, so it should always be valid outside of development testing. Thus, it is assumed that all errors from here on are the fault of an external file.
    if (json_colours.isNull() || !json_colours.isObject() || !json_colours["colours"].isObject() || !json_colours["colours"]["presets"].isArray() || !json_colours["colours"]["values"].isArray()) {
        if (force_internal) { // This check helps prevent infinite looping during development or in the event something goes really, really wrong.
            return false;
        }
        Utility::error("External colours.json file failed structure check! File will be ignored.");
        return load_colours(true);
    }

    // Get presets and colours.
    {
        for (QJsonValue item : json_colours["colours"]["presets"].toArray())
            if(!item.isString())
                Utility::error("Bad preset data. Entry will be skipped.");
            else
                presets.append(item.toString());
        presets.sort();
        presets.insert(0, "NONE");

        for (QJsonValue item : json_colours["colours"]["values"].toArray())
            if(!item.isString())
                Utility::error("Bad colour data. Entry will be skipped.");
            else
                colours.append(item.toString());
        colours.sort();
    }

    return true;
}

void ColoursWidget::whats_this()
{
    // Change "What's This?" text based on ui state.
    if (showing_colours)
        ui->listWidget->setWhatsThis("I am in colour list mode.\nClick on an entry in the widget to toggle its selection state. Highlighted items are the selected items.");
    else
        ui->listWidget->setWhatsThis("I am in preset selection mode.\nClick on an entry in the widget to set it to the selected item. The highlighted item is the selected item.");

    QWhatsThis::enterWhatsThisMode();
}

void ColoursWidget::closeEvent(QCloseEvent *event)
{
    // Overrides the close event. Instead of closing the widget, we just hide it and show the parent.

    event->ignore();
    parentWidget()->show();
    hide();
}

void ColoursWidget::show_presets()
{
    // Populates the widget with preset flags

    ui->listWidget->clear();
    ui->listWidget->addItems(presets);
    ui->listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    showing_colours = false;
}

void ColoursWidget::show_colours()
{
    // Populates the widget with colour flags

    ui->listWidget->clear();
    ui->listWidget->addItems(colours);
    ui->listWidget->setSelectionMode(QAbstractItemView::MultiSelection);
    showing_colours = true;
}

void ColoursWidget::apply()
{
    // Applies changes to data then hides the widget and shows the parent.

    if (!ui->listWidget->selectedItems().empty()) {
        if (!showing_colours)
            colour_ptr->setPreset(ui->listWidget->selectedItems()[0]->text());
        else {
            QStringList colours = QStringList();
            for (QListWidgetItem *entry : ui->listWidget->selectedItems())
                colours.append(entry->text());
            colour_ptr->setColours(colours);
        }
    } else colour_ptr->setPreset("NONE");

    parentWidget()->show();
    hide();
}

QStringList ColoursWidget::colours = QStringList(), ColoursWidget::presets = QStringList();
