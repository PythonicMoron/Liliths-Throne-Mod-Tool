#include "tattoowindow.h"
#include "ui_tattoowindow.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QFileDialog>

#include "utility.h"
#include "tattoo/availabilitycomboboxdelegate.h"

TattooWindow::TattooWindow(const QDomDocument &xml_doc, const QString &path, QWidget *parent) : QWidget(parent), ui(new Ui::TattooWindow)
{
    // Setup
    ui->setupUi(this);
    setWindowFlag(Qt::Window);
    setAttribute(Qt::WA_DeleteOnClose);
    setFixedSize(TattooWindow::size());

    // Widgets and widget handlers
    colours_widget = new ColoursWidget(this);
    availability_handler = new ListViewHandler(ui->availabilityList, data.slot_availability, new AvailabilityComboBoxDelegate(this));

    // Populate Ui
    populate_ui();



    // Begin ui connections. Warning: Some of these are very much not pretty.

    // Connections for misc/loose items
    connect(ui->saveButton, &QPushButton::released, [this] () {save(false);});
    connect(ui->saveAsButton, &QPushButton::released, [this] () {save(true);});

    // Connections for text tab
    connect(ui->nameEdit, &QLineEdit::textChanged, [this] (const QString &text) {data.name = text;});
    connect(ui->descriptionTextEdit, &QPlainTextEdit::textChanged, [this] () {data.description = ui->descriptionTextEdit->toPlainText();});

    // Connections for everything else tab
    connect(ui->valueSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [this] (int value) {data.value = value;});
    connect(ui->imageNameEdit, &QLineEdit::textChanged, [this] (const QString &text) {data.image_name = text;});
    connect(ui->effectLimitSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [this] (int value) {data.enchantment_limit = value;});
    connect(ui->pColButton, &QPushButton::released, [this]() {colours_widget->open(data.primary_colour.get(), ui->pColCheckBox->isChecked());});
    connect(ui->sColButton, &QPushButton::released, [this]() {colours_widget->open(data.secondary_colour.get(), ui->sColCheckBox->isChecked());});
    connect(ui->tColButton, &QPushButton::released, [this]() {colours_widget->open(data.tertiary_colour.get(), ui->tColCheckBox->isChecked());});
    connect(ui->availabilityList, &QListView::customContextMenuRequested, [this] (const QPoint &pos) {availability_handler->contex_menu(pos);});

    // End ui connections.



    // Error string
    QString err = QString();

    // Attempt to load xml data. Fail if read_file returns false.
    if (!data.read_file(xml_doc, err)) {
        Utility::error(err);
        this->close();
        return;
    }

    // Report errors, if any.
    if (!err.isEmpty())
        Utility::error(err);

    // Set window titles to help differentiate windows, set save location, and then handle save button all depending on if this was constructed with a path or not (determines if new file or loaded file)
    if (path.isNull() || path.isEmpty()) {
        set_titles("New");
        location = QString();

        // Disable save button
        ui->saveButton->setEnabled(false);
    } else {
        set_titles(QFileInfo(location).fileName());
        location = path;

        // Enable save button
        ui->saveButton->setEnabled(true);
    }

    // Update ui with new field data
    update_ui();
}

TattooWindow::~TattooWindow()
{
    // Handlers
    delete availability_handler;

    // Ui
    delete ui;

    // Child widgets
    delete colours_widget;
}

bool TattooWindow::load_defs(bool force_internal)
{
    // This function loads the data from the defs json file and if successful, replaces the static ui_data object.
    // You MUST call populate_ui to apply changes.

    // Returns succuss state (only returns false if both internal and external files failed to load).
    // The force_internal parameter tells the function to ignore external files, if any, when set to true. Defaults to false.

    QFile file("tattoo_defs.json"); // First, check for external file. Allows for overriding internal resource.
    if (!file.exists() || force_internal) {
        file.setFileName(":/res/data_files/tattoo_defs.json"); // If no external file exists or this was called with force_internal set to true, load internal file.
        if (!file.exists()) {
            Utility::error("Could not open a valid tattoo_defs.json file!"); // Something or someone really fucked up.
            return false;
        }
    }

    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString data = file.readAll();
    file.close();
    QJsonDocument defs = QJsonDocument::fromJson(data.toUtf8());

     // Temporary ui data object. If loading fails, the object will be deleted and no previous ui data, if any, will be lost.
    auto temp_ui_data = QSharedPointer<TattooWindow::UiData>(new TattooWindow::UiData());

    // Lambda to simplify critical errors
    auto fail = [force_internal] (const QString &fail_message) {
        if (force_internal) { // This check helps prevent infinite looping during development or in the event something goes really, really wrong.
            return false;
        }
        Utility::error(fail_message);
        return load_defs(true); // Try again with force internal true
    };

    // The internal file is compiled in and unable to be edited, so it should always be valid outside of development testing. Thus, it is assumed that all errors from here on are the fault of an external file.
    if (defs.isNull() || !defs.isObject() || !defs["tattoo_data"].isObject() || !defs["tattoo_data"]["field_data"].isObject())
        return fail("External file failed structure check! File will be ignored.");


    // Get spin box ranges
    if (defs["tattoo_data"]["ranges"].isObject()) {
        if (defs["tattoo_data"]["ranges"]["value"].isArray())
            temp_ui_data->value_range = QPair<int,int>(defs["weapon_data"]["ranges"]["value"][0].toInt(-1), defs["tattoo_data"]["ranges"]["value"][1].toInt(-1));
        else Utility::error("Failed to read integer range data for value input. Default values will be used.");

        if (defs["tattoo_data"]["ranges"]["enchantment_limit"].isArray())
            temp_ui_data->enchantment_limit_range = QPair<int,int>(defs["tattoo_data"]["ranges"]["enchantment_limit"][0].toInt(-1), defs["tattoo_data"]["ranges"]["enchantment_limit"][1].toInt(-1));
        else Utility::error("Failed to read integer range data for enchantment limit input. Default values will be used.");

    } else Utility::error("Failed to read integer range data. All ranges will be set to default values.");


    // Get slot entries.
    if (!defs["tattoo_data"]["field_data"]["slot"].isArray())
        return fail("External file failed slot data check! File will be ignored.");

    for (QJsonValue item : defs["tattoo_data"]["field_data"]["slot"].toArray())
        if(!item.isString())
            Utility::error("Bad slot data. Entry will be skipped.");
        else
            temp_ui_data->inventory_slots.append(item.toString());
    temp_ui_data->inventory_slots.sort();


    // Set new data. QSharedPointer should delete discarded data.
    ui_data = temp_ui_data;

    // Setup delegates
    AvailabilityComboBoxDelegate::setup(ui_data->inventory_slots);

    // Done
    return true;
}

void TattooWindow::populate_ui()
{
    // This re/populates the ui with data from the ui_data object.
    // This function should only be called after changes are made to the ui_data object (generally via load_defs).
    // You may want to call update_ui after calling this to reload the mod data into the ui as this will reset most fields.

    // Reset child widgets
    colours_widget->reload_ui();

    // Setup spin box ranges.

    // Nice lambda do do some checking for each range pair
    auto set_range = [] (QSpinBox *spin_box, const QPair<int,int> &pair) {
        int min, max;

        if (pair.first == -1)
            min = spin_box->minimum();
        else
            min = pair.first;

        if (pair.second == -1)
            max = spin_box->maximum();
        else
            max = pair.second;

        spin_box->setRange(min, max);
    };

    set_range(ui->valueSpinBox, ui_data->value_range);
    set_range(ui->effectLimitSpinBox, ui_data->enchantment_limit_range);
}

void TattooWindow::save(bool as)
{
    // This handles the save dialog and the basic save logic. Most of the heavy lifting is done in TattooMod::save_file.

    auto err = QString();

    // Save as...
    if (as) {
        location = QFileDialog::getSaveFileName(this, "Save clothing mod", "./", "(*.xml)");

        // Should trigger on cancel or failure
        if (location.isEmpty() || location.isNull()) {
            Utility::error("Failed to save file!");
            return;
        }

        // If all went well, the save location is valid. Enable the save button.
        ui->saveButton->setEnabled(true);

        // Save
        if (!data.save_file(location, err))
            Utility::error(err);
        else
            set_titles(QFileInfo(location).fileName());

    // Save
    } else {
        if (!location.isNull() && !location.isEmpty())
            // Save
            if (!data.save_file(location, err))
                Utility::error(err);
    }
}

void TattooWindow::set_titles(const QString &title)
{
    // You can figure this one out on your own.

    setWindowTitle(title);
    colours_widget->setWindowTitle(title);
}

void TattooWindow::update_ui()
{
    // This goes through the TattooMod object (named 'data') and applies all its data to the ui fields.

    // Update spin boxes
    ui->valueSpinBox->setValue(data.value);
    ui->effectLimitSpinBox->setValue(data.enchantment_limit);


    // Quick lambda for setting the colour preset checkboxes
    const auto update_colour_toggle = [] (Colour *colour, QCheckBox *toggle) {
        if (colour->getType() == Colour::Type::preset)
            toggle->setCheckState(Qt::CheckState::Checked);
        else toggle->setCheckState(Qt::CheckState::Unchecked);
    };

    update_colour_toggle(data.primary_colour.get(), ui->pColCheckBox);
    update_colour_toggle(data.secondary_colour.get(), ui->sColCheckBox);
    update_colour_toggle(data.tertiary_colour.get(), ui->tColCheckBox);


    // Update line edits
    ui->nameEdit->setText(data.name);
    ui->imageNameEdit->setText(data.image_name);


    // Update plain text edits
    ui->descriptionTextEdit->setPlainText(data.description);


    // Misc updates
    availability_handler->update();
}

TattooWindow::UiData::UiData()
{
    value_range = QPair<int,int>(-1,-1);
    enchantment_limit_range = QPair<int,int>(-1,-1);
    inventory_slots = QStringList();
}

QSharedPointer<TattooWindow::UiData> TattooWindow::ui_data = QSharedPointer<TattooWindow::UiData>();
