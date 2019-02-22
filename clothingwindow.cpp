#include "clothingwindow.h"
#include "ui_clothingwindow.h"

#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonArray>

#include "utility.h"
#include "clothing/treecomboboxdelegate.h"
#include "clothing/slotcomboboxdelegate.h"

ClothingWindow::ClothingWindow(const QDomDocument &xml_doc, const QString &path, QWidget *parent) : QWidget(parent), ui(new Ui::ClothingWindow)
{
    // Setup
    ui->setupUi(this);
    setWindowFlag(Qt::Window);
    setAttribute(Qt::WA_DeleteOnClose);
    setFixedSize(ClothingWindow::size());

    // Widgets and widget handlers
    colours_widget = new ColoursWidget(this);
    tags_widget = new TagsWidget(data.item_tags, this);
    dialogue_widget = new DialogueWidget(this);
    effect_widget = new EnchantmentWidget(EnchantmentWidget::Mode::clothing, this);
    tree_handler = new TreeHandler(ui->accessTree, data.blocked_parts);
    effects_list_handler = new EffectListHandler(ui->enchantmentList);
    incompatible_slot_handler = new ListViewHandler(ui->incompatibleSlotList, data.incompatible_slots, new SlotComboBoxDelegate(this));

    // Populate Ui
    populate_ui();



    // Begin ui connections. Warning: Some of these are very much not pretty.

    // Connections for misc/loose items
    connect(ui->saveButton, &QPushButton::released, [this] () {save(false);});
    connect(ui->saveAsButton, &QPushButton::released, [this] () {save(true);});

    // Connections for general tab
    connect(ui->nameEdit, &QLineEdit::textChanged, [this] (const QString &text) {data.name = text;});
    connect(ui->valueInput, QOverload<int>::of(&QSpinBox::valueChanged), [this] (int value) {data.value = value;});
    connect(ui->slotComboBox, QOverload<const QString &>::of(&QComboBox::currentIndexChanged), [this] (const QString &text) {data.slot = text;});
    connect(ui->genderComboBox, QOverload<const QString &>::of(&QComboBox::currentIndexChanged), [this] (const QString &text) {data.femininity = text;});
    connect(ui->rarityComboBox, QOverload<const QString &>::of(&QComboBox::currentIndexChanged), [this] (const QString &text) {data.rarity = text;});
    connect(ui->resistanceInput, QOverload<int>::of(&QSpinBox::valueChanged), [this] (int value) {data.physical_resistance = value;});
    connect(ui->setComboBox, QOverload<const QString &>::of(&QComboBox::currentIndexChanged), [this] (const QString &text) {data.clothing_set = text;});
    connect(ui->tagsButton, &QPushButton::released, [this] () {tags_widget->open();});
    connect(ui->incompatibleSlotList, &QListView::customContextMenuRequested, [this] (const QPoint &pos) {incompatible_slot_handler->contex_menu(pos);});

    // Connections for accessibility tab
    connect(ui->accessTree, &QTreeView::customContextMenuRequested, [this] (const QPoint &pos) {tree_handler->contex_menu(pos);});

    // Connections for enchantments tab
    connect(ui->enchantmentLimitInput, QOverload<int>::of(&QSpinBox::valueChanged), [this] (int value) {data.enchantment_limit = value;});
    connect(ui->enchantmentList, &QListWidget::customContextMenuRequested, [this] (const QPoint &pos) {effects_list_handler->contex_menu(pos, ui->enchantmentLimitInput->value());});
    connect(effects_list_handler, &EffectListHandler::remove, [this] (int index) {data.effects.removeAt(index); effects_list_handler->update(data.effects);});
    connect(effects_list_handler, &EffectListHandler::add, [this] () {data.effects.append(DataCommon::Effect()); effect_widget->open(&data.effects.last());});
    connect(effect_widget, &EnchantmentWidget::finished, [this] () {effects_list_handler->update(data.effects);});
    connect(ui->enchantmentList, &QListWidget::itemDoubleClicked, [this] (QListWidgetItem* UNUSED(item)) {effect_widget->open(&data.effects[ui->enchantmentList->currentRow()]);});

    // Connections for dialogue tab
    connect(ui->pluralNameEdit, &QLineEdit::textChanged, [this] (const QString &text) {data.plural_name = text;});
    connect(ui->pluralToggle, &QCheckBox::stateChanged, [this] () {data.plural_default = ui->pluralToggle->isChecked();});
    connect(ui->determinerEdit, &QLineEdit::textChanged, [this] (const QString &text) {data.determiner = text;});
    connect(ui->descriptionEdit, &QPlainTextEdit::textChanged, [this] () {data.description = ui->descriptionEdit->toPlainText();});
    connect(ui->advancedDialogueButton, &QPushButton::released, [this] () {dialogue_widget->open(data.dialogue);});

    // Connections for colour tab
    connect(ui->pColButton, &QPushButton::released, [this]() {colours_widget->open(data.primary_colour.get(), ui->pColToggle->isChecked());});
    connect(ui->pColDyeButton, &QPushButton::released, [this] () {colours_widget->open(data.primary_colour_dye.get(), ui->pColDyeToggle->isChecked());});
    connect(ui->sColButton, &QPushButton::released, [this] () {colours_widget->open(data.secondary_colour.get(), ui->sColToggle->isChecked());});
    connect(ui->sColDyeButton, &QPushButton::released, [this] () {colours_widget->open(data.secondary_colour_dye.get(), ui->sColDyeToggle->isChecked());});
    connect(ui->tColButton, &QPushButton::released, [this] () {colours_widget->open(data.tertiary_colour.get(), ui->tColToggle->isChecked());});
    connect(ui->tColDyeButton, &QPushButton::released, [this] () {colours_widget->open(data.tertiary_colour_dye.get(), ui->tColDyeToggle->isChecked());});
    connect(ui->imageEdit, &QLineEdit::textChanged, [this] (const QString &text) {data.image_name = text;});
    connect(ui->equippedImageEdit, &QLineEdit::textChanged, [this] (const QString &text) {data.equipped_image_name = text;});

    // End ui connections



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

ClothingWindow::~ClothingWindow()
{
    // Handlers
    delete tree_handler;
    delete effects_list_handler;
    delete incompatible_slot_handler;

    // Ui
    delete ui;

    // Child widgets
    delete colours_widget;
    delete tags_widget;
    delete dialogue_widget;
    delete effect_widget;
}

bool ClothingWindow::load_defs(bool force_internal)
{
    // This function loads the data from the defs json file and if successful, replaces the static ui_data object.
    // You MUST call populate_ui to apply changes.

    // Returns succuss state (only returns false if both internal and external files failed to load).
    // The force_internal parameter tells the function to ignore external files, if any, when set to true. Defaults to false.

    QFile file("clothing_defs.json"); // First, check for external file. Allows for overriding internal resource.
    if (!file.exists() || force_internal) {
        file.setFileName(":/res/data_files/clothing_defs.json"); // If no external file exists or this was called with force_internal set to true, load internal file.
        if (!file.exists()) {
            Utility::error("Could not open a valid clothing_defs.json file!"); // Something or someone really fucked up.
            return false;
        }
    }

    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString data = file.readAll();
    file.close();
    QJsonDocument defs = QJsonDocument::fromJson(data.toUtf8());

    // Temporary ui data object. If loading fails, the object will be deleted and no previous ui data, if any, will be lost.
    auto temp_ui_data = QSharedPointer<ClothingWindow::UiData>(new ClothingWindow::UiData());

    // Lambda to simplify critical errors
    auto fail = [force_internal] (const QString &fail_message) {
        if (force_internal) { // This check helps prevent infinite looping during development or in the event something goes really, really wrong.
            return false;
        }
        Utility::error(fail_message);
        return load_defs(true); // Try again with force internal true
    };

    // The internal file is compiled in and unable to be edited, so it should always be valid outside of development testing. Thus, it is assumed that all errors from here on are the fault of an external file.
    if (defs.isNull() || !defs.isObject() || !defs["clothing_data"].isObject() || !defs["clothing_data"]["field_data"].isObject())
        return fail("External file failed structure check! File will be ignored.");


    // Get spin box ranges.
    if (defs["clothing_data"]["ranges"].isObject()) {
        if (defs["clothing_data"]["ranges"]["value"].isArray())
            temp_ui_data->value_range = QPair<int,int>(defs["clothing_data"]["ranges"]["value"][0].toInt(-1), defs["clothing_data"]["ranges"]["value"][1].toInt(-1));
        else Utility::error("Failed to read integer range data for value input. Default values will be used.");

        if (defs["clothing_data"]["ranges"]["physical_resistance"].isArray())
            temp_ui_data->physical_resistance_range = QPair<int,int>(defs["clothing_data"]["ranges"]["physical_resistance"][0].toInt(-1), defs["clothing_data"]["ranges"]["physical_resistance"][1].toInt(-1));
        else Utility::error("Failed to read integer range data for physical resistance input. Default values will be used.");

        if (defs["clothing_data"]["ranges"]["enchantment_limit"].isArray())
            temp_ui_data->enchantment_limit_range = QPair<int,int>(defs["clothing_data"]["ranges"]["enchantment_limit"][0].toInt(-1), defs["clothing_data"]["ranges"]["enchantment_limit"][1].toInt(-1));
        else Utility::error("Failed to read integer range data for enchantment limit input. Default values will be used.");

    } else Utility::error("Failed to read integer range data. All ranges will be set to default values.");


    // Get slot entries.
    if (!defs["clothing_data"]["field_data"]["slot"].isArray())
        return fail("External file failed slot data check! File will be ignored.");

    for (QJsonValue item : defs["clothing_data"]["field_data"]["slot"].toArray())
        if(!item.isString())
            Utility::error("Bad slot data. Entry will be skipped.");
        else
            temp_ui_data->slot_list.append(item.toString());
    temp_ui_data->slot_list.sort();


    // Get femininity entries.
    if (!defs["clothing_data"]["field_data"]["femininity"].isArray())
        return fail("External file failed femininity data check! File will be ignored.");

    for (QJsonValue item : defs["clothing_data"]["field_data"]["femininity"].toArray())
        if(!item.isString())
            Utility::error("Bad femininity data. Entry will be skipped.");
        else
            temp_ui_data->gender_list.append(item.toString());
    temp_ui_data->gender_list.sort();


    // Get rarity entries.
    if (!defs["clothing_data"]["field_data"]["rarity"].isArray())
        return fail("External file failed rarity data check! File will be ignored.");

    for (QJsonValue item : defs["clothing_data"]["field_data"]["rarity"].toArray())
        if(!item.isString())
            Utility::error("Bad rarity data. Entry will be skipped.");
        else
            temp_ui_data->rarity_list.append(item.toString());
    //temp_ui_data->rarity_list.sort(); // Rarity list is already sorted in file so no need to sort. Left line here in case I change my mind.


    // Get clothing set entries.
    if (!defs["clothing_data"]["field_data"]["clothing_set"].isArray())
        return fail("External file failed clothing set data check! File will be ignored.");

    for (QJsonValue item : defs["clothing_data"]["field_data"]["clothing_set"].toArray())
        if(!item.isString())
            Utility::error("Bad clothing set data. Entry will be skipped.");
        else
            temp_ui_data->clothing_set_list.append(item.toString());
    temp_ui_data->clothing_set_list.sort();
    temp_ui_data->clothing_set_list.insert(0, "NULL");


    // Start accessibility setup.
    if (!defs["clothing_data"]["field_data"]["access"].isObject())
        return fail("External file failed access structure check! File will be ignored.");
    {
        // Type entries
        if (!defs["clothing_data"]["field_data"]["access"]["type"].isArray())
            return fail("External file failed access type data check! File will be ignored.");

        for (QJsonValue item : defs["clothing_data"]["field_data"]["access"]["type"].toArray())
            if(!item.isString())
                Utility::error("Bad access type data. Entry will be skipped.");
            else
                temp_ui_data->displacement_types.append(item.toString());
        temp_ui_data->displacement_types.sort();

        // Access slot entries
        if (!defs["clothing_data"]["field_data"]["access"]["clothing_access"].isArray())
            return fail("External file failed clothing access data check! File will be ignored.");

        for (QJsonValue item : defs["clothing_data"]["field_data"]["access"]["clothing_access"].toArray())
            if(!item.isString())
                Utility::error("Bad clothing access data. Entry will be skipped.");
            else
                temp_ui_data->access_slots.append(item.toString());
        temp_ui_data->access_slots.sort();

        // Blocked bodypart entries
        if (!defs["clothing_data"]["field_data"]["access"]["blocked_bodyparts"].isArray())
            return fail("External file failed blocked bodyparts data check! File will be ignored.");

        for (QJsonValue item : defs["clothing_data"]["field_data"]["access"]["blocked_bodyparts"].toArray())
            if(!item.isString())
                Utility::error("Bad blocked bodypart data. Entry will be skipped.");
            else
                temp_ui_data->blocked_bodyparts.append(item.toString());
        temp_ui_data->blocked_bodyparts.sort();

        // Concealed preset entries
        if (!defs["clothing_data"]["field_data"]["access"]["concealed_presets"].isArray())
            return fail("External file failed concealed presets data check! File will be ignored.");

        for (QJsonValue item : defs["clothing_data"]["field_data"]["access"]["concealed_presets"].toArray())
            if(!item.isString())
                Utility::error("Bad concealed preset data. Entry will be skipped.");
            else
                temp_ui_data->concealed_presets.append(item.toString());
        temp_ui_data->concealed_presets.sort();
    }


    // Set new data. QSharedPointer should delete discarded data.
    ui_data = temp_ui_data;

    // Setup delegates
    SlotComboBoxDelegate::setup(ui_data->slot_list);
    DialogueWidget::setup(ui_data->displacement_types);
    TreeComboBoxDelegate::setup(ui_data->displacement_types, ui_data->access_slots, ui_data->blocked_bodyparts, ui_data->slot_list, ui_data->concealed_presets);

    // Done
    return true;
}

void ClothingWindow::populate_ui()
{
    // This re/populates the ui with data from the ui_data object.
    // This function should only be called after changes are made to the ui_data object (generally via load_defs).
    // You may want to call update_ui after calling this to reload the mod data into the ui as this will reset most fields.

    // Reset child widgets
    tags_widget->reload_ui();
    effect_widget->reload_ui();
    dialogue_widget->reload_ui();
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

    set_range(ui->valueInput, ui_data->value_range);
    set_range(ui->resistanceInput, ui_data->physical_resistance_range);
    set_range(ui->enchantmentLimitInput, ui_data->enchantment_limit_range);

    // Setup combo boxes.

    // Simple lambda to save my fingers some wear
    auto setup = [] (QComboBox *combo_box, const QStringList &list) {
        combo_box->blockSignals(true);
        combo_box->clear();
        combo_box->addItems(list);
        combo_box->blockSignals(false);
    };

    setup(ui->slotComboBox, ui_data->slot_list);
    setup(ui->genderComboBox, ui_data->gender_list);
    setup(ui->rarityComboBox, ui_data->rarity_list);
    setup(ui->setComboBox, ui_data->clothing_set_list);
}

void ClothingWindow::save(bool as)
{
    // This handles the save dialog and the basic save logic. Most of the heavy lifting is done in ClothingMod::save_file.

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

void ClothingWindow::set_titles(const QString &title)
{
    // You can figure this one out on your own.

    setWindowTitle(title);
    colours_widget->setWindowTitle(title);
    tags_widget->setWindowTitle(title);
    dialogue_widget->setWindowTitle(title);
    effect_widget->setWindowTitle(title);
}

void ClothingWindow::update_ui()
{
    // This goes through the ClothingMod object (named 'data') and applies all its data to the ui fields.

    // Update spin boxes
    ui->valueInput->setValue(data.value); // Value
    ui->resistanceInput->setValue(data.physical_resistance); // Physical resistance
    ui->enchantmentLimitInput->setValue(data.enchantment_limit); // Enchantment limit


    // Update check boxes
    if (data.plural_default) // Plural by default toggle
        ui->pluralToggle->setCheckState(Qt::CheckState::Checked);
    else ui->pluralToggle->setCheckState(Qt::CheckState::Unchecked);

    // Quick lambda for setting the colour preset checkboxes
    const auto update_colour_toggle = [] (Colour *colour, QCheckBox *toggle) {
        if (colour->getType() == Colour::Type::preset)
            toggle->setCheckState(Qt::CheckState::Checked);
        else toggle->setCheckState(Qt::CheckState::Unchecked);
    };

    update_colour_toggle(data.primary_colour.get(), ui->pColToggle); // Primary colour preset
    update_colour_toggle(data.primary_colour_dye.get(), ui->pColDyeToggle); // Primary colour dye preset

    update_colour_toggle(data.secondary_colour.get(), ui->sColToggle); // Secondary colour preset
    update_colour_toggle(data.secondary_colour_dye.get(), ui->sColDyeToggle); // Secondary colour dye preset

    update_colour_toggle(data.tertiary_colour.get(), ui->tColToggle); // Tertiary colour preset
    update_colour_toggle(data.tertiary_colour_dye.get(), ui->tColDyeToggle); // Tertiary colour dye preset


    // Update combo box selections
    ui->slotComboBox->setCurrentIndex(ui->slotComboBox->findText(data.slot));
    ui->genderComboBox->setCurrentIndex(ui->genderComboBox->findText(data.femininity));
    ui->rarityComboBox->setCurrentIndex(ui->rarityComboBox->findText(data.rarity));
    ui->setComboBox->setCurrentIndex(ui->setComboBox->findText(data.clothing_set));

    // Update line edit text
    ui->nameEdit->setText(data.name);
    ui->pluralNameEdit->setText(data.plural_name);
    ui->determinerEdit->setText(data.determiner);
    ui->imageEdit->setText(data.image_name);
    ui->equippedImageEdit->setText(data.equipped_image_name);


    // Update plain text edit
    ui->descriptionEdit->setPlainText(data.description);


    // Misc updates
    tags_widget->update();
    tree_handler->update();
    effects_list_handler->update(data.effects);
    incompatible_slot_handler->update();
}

ClothingWindow::UiData::UiData()
{
    value_range = QPair<int,int>(-1,-1);
    physical_resistance_range = QPair<int,int>(-1,-1);
    enchantment_limit_range = QPair<int,int>(-1,-1);
    slot_list = QStringList();
    gender_list = QStringList();
    rarity_list = QStringList();
    clothing_set_list = QStringList();
    displacement_types = QStringList();
    access_slots = QStringList();
    blocked_bodyparts = QStringList();
    concealed_presets = QStringList();
}

QSharedPointer<ClothingWindow::UiData> ClothingWindow::ui_data = QSharedPointer<ClothingWindow::UiData>();
