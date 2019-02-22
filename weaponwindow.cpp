#include "weaponwindow.h"
#include "ui_weaponwindow.h"

#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonArray>

#include "utility.h"
#include "weapon/damagecomboboxdelegate.h"
#include "weapon/spellcomboboxdelegate.h"
#include "weapon/texteditdelegate.h"

WeaponWindow::WeaponWindow(const QDomDocument &xml_doc, const QString &path, QWidget *parent) : QWidget(parent), ui(new Ui::WeaponWindow)
{
    // Setup
    ui->setupUi(this);
    setWindowFlag(Qt::Window);
    setAttribute(Qt::WA_DeleteOnClose);
    setFixedSize(WeaponWindow::size());

    // Widgets and widget handlers
    colours_widget = new ColoursWidget(this);
    tags_widget = new TagsWidget(data.item_tags, this);
    effects_widget = new EnchantmentWidget(EnchantmentWidget::Mode::weapon, this);
    effects_list_handler = new EffectListHandler(ui->effectList);
    damage_type_list_handler = new ListViewHandler(ui->damageTypesList, data.damage_types, new DamageComboBoxDelegate(this));
    spell_list_handler = new ListViewHandler(ui->spellList, data.spells, new SpellComboBoxDelegate(this));
    hit_text_list_handler = new ListViewHandler(ui->hitList, data.hit_text, new TextEditDelegate(this));
    miss_text_list_handler = new ListViewHandler(ui->missList, data.miss_text, new TextEditDelegate(this));

    // Populate Ui
    populate_ui();



    // Begin ui connections. Warning: Some of these are very much not pretty.

    // Connections for misc/loose items
    connect(ui->saveButton, &QPushButton::released, [this] () {save(false);});
    connect(ui->saveAsButton, &QPushButton::released, [this] () {save(true);});

    // Connections for general tab
    connect(ui->nameEdit, &QLineEdit::textChanged, [this] (const QString &text) {data.name = text;});
    connect(ui->valueSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [this] (int value) {data.value = value;});
    connect(ui->meleeCheckBox, &QCheckBox::stateChanged, [this] {data.melee = ui->meleeCheckBox->isChecked();});
    connect(ui->twoHandedCheckBox, &QCheckBox::stateChanged, [this] () {data.two_handed = ui->twoHandedCheckBox->isChecked();});
    connect(ui->rarityComboBox, QOverload<const QString &>::of(&QComboBox::currentIndexChanged), [this] (const QString &text) {data.rarity = text;});
    connect(ui->damageSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [this] (int value) {data.damage = value;});
    connect(ui->arcaneCostSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [this] (int value) {data.arcane_cost = value;});
    connect(ui->varianceComboBox, QOverload<const QString &>::of(&QComboBox::currentIndexChanged), [this] (const QString &text) {data.damage_variance = text;});
    connect(ui->damageTypesList, &QListView::customContextMenuRequested, [this] (const QPoint &pos) {damage_type_list_handler->contex_menu(pos);});
    connect(ui->tagsButton, &QPushButton::released, [this] () {tags_widget->open();});

    // Connections for text tab
    connect(ui->pluralNameEdit, &QLineEdit::textChanged, [this] (QString text) {data.name_plural = text;});
    connect(ui->pluralCheckBox, &QCheckBox::stateChanged, [this] () {data.plural_default = ui->pluralCheckBox->isChecked();});
    connect(ui->determinerEdit, &QLineEdit::textChanged, [this] (const QString &text) {data.determiner = text;});
    connect(ui->descriptionEdit, &QPlainTextEdit::textChanged, [this] () {data.description = ui->descriptionEdit->toPlainText();});

    // Connections for advanced text tab
    connect(ui->descriptorEdit, &QLineEdit::textChanged, [this] (const QString &text) {data.attack_descriptor = text;});
    connect(ui->tooltipTextEdit, &QPlainTextEdit::textChanged, [this] () {data.attack_tooltip = ui->tooltipTextEdit->toPlainText();});
    connect(ui->equipTextEdit, &QPlainTextEdit::textChanged, [this] () {data.equip_text = ui->equipTextEdit->toPlainText();});
    connect(ui->unequipTextEdit, &QPlainTextEdit::textChanged, [this] () {data.unequip_text = ui->unequipTextEdit->toPlainText();});

    // Connections for hit/miss text tab
    connect(ui->hitList, &QListView::customContextMenuRequested, [this] (const QPoint &pos) {hit_text_list_handler->contex_menu(pos);});
    connect(ui->missList, &QListView::customContextMenuRequested, [this] (const QPoint &pos) {miss_text_list_handler->contex_menu(pos);});

    // Connections for arcane tab
    connect(ui->effectLimitSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [this] (int value) {data.enchantment_limit = value;});
    connect(ui->effectList, &QListWidget::customContextMenuRequested, [this] (const QPoint &pos) {effects_list_handler->contex_menu(pos, ui->effectLimitSpinBox->value());});
    connect(effects_list_handler, &EffectListHandler::remove, [this] (int index) {data.effects.removeAt(index); effects_list_handler->update(data.effects);});
    connect(effects_list_handler, &EffectListHandler::add, [this] () {data.effects.append(DataCommon::Effect()); effects_widget->open(&data.effects.last());});
    connect(effects_widget, &EnchantmentWidget::finished, [this] () {effects_list_handler->update(data.effects);});
    connect(ui->effectList, &QListWidget::itemDoubleClicked, [this] (QListWidgetItem* UNUSED(item)) {effects_widget->open(&data.effects[ui->effectList->currentRow()]);});
    connect(ui->spellList, &QListView::customContextMenuRequested, [this] (const QPoint &pos) {spell_list_handler->contex_menu(pos);});

    // Connections for visuals tab
    connect(ui->pColButton, &QPushButton::released, [this]() {colours_widget->open(data.primary_colour.get(), ui->pColPresetCheckBox->isChecked());});
    connect(ui->pColDyeButton, &QPushButton::released, [this]() {colours_widget->open(data.primary_colour_dye.get(), ui->pColDyePresetCheckBox->isChecked());});
    connect(ui->sColButton, &QPushButton::released, [this]() {colours_widget->open(data.secondary_colour.get(), ui->sColDyePresetCheckBox->isChecked());});
    connect(ui->sColDyeButton, &QPushButton::released, [this]() {colours_widget->open(data.secondary_colour_dye.get(), ui->sColDyePresetCheckBox->isChecked());});
    connect(ui->imageNameEdit, &QLineEdit::textChanged, [this] (const QString &text) {data.image_name = text;});
    connect(ui->equippedImageEdit, &QLineEdit::textChanged, [this] (const QString &text) {data.equipped_image_name = text;});

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

WeaponWindow::~WeaponWindow()
{
    // Handlers
    delete effects_list_handler;
    delete damage_type_list_handler;
    delete spell_list_handler;
    delete hit_text_list_handler;
    delete miss_text_list_handler;

    // Ui
    delete ui;

    // Child widgets
    delete effects_widget;
    delete tags_widget;
    delete colours_widget;
}

bool WeaponWindow::load_defs(bool force_internal)
{
    // This function loads the data from the defs json file and if successful, replaces the static ui_data object.
    // You MUST call populate_ui to apply changes.

    // Returns succuss state (only returns false if both internal and external files failed to load).
    // The force_internal parameter tells the function to ignore external files, if any, when set to true. Defaults to false.

    QFile file("weapon_defs.json"); // First, check for external file. Allows for overriding internal resource.
    if (!file.exists() || force_internal) {
        file.setFileName(":/res/data_files/weapon_defs.json"); // If no external file exists or this was called with force_internal set to true, load internal file.
        if (!file.exists()) {
            Utility::error("Could not open a valid weapon_defs.json file!"); // Something or someone really fucked up.
            return false;
        }
    }

    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString data = file.readAll();
    file.close();
    QJsonDocument defs = QJsonDocument::fromJson(data.toUtf8());

    // Temporary ui data object. If loading fails, the object will be deleted and no previous ui data, if any, will be lost.
    auto temp_ui_data = QSharedPointer<UiData>(new WeaponWindow::UiData());

    // Lambda to simplify critical errors
    auto fail = [force_internal] (const QString &fail_message) {
        if (force_internal) { // This check helps prevent infinite looping during development or in the event something goes really, really wrong.
            return false;
        }
        Utility::error(fail_message);
        return load_defs(true); // Try again with force internal true
    };

    // The internal file is compiled in and unable to be edited, so it should always be valid outside of development testing. Thus, it is assumed that all errors from here on are the fault of an external file.
    if (defs.isNull() || !defs.isObject() || !defs["weapon_data"].isObject() || !defs["weapon_data"]["field_data"].isObject())
        return fail("External file failed structure check! File will be ignored.");


    // Get spin box ranges
    if (defs["weapon_data"]["ranges"].isObject()) {
        if (defs["weapon_data"]["ranges"]["value"].isArray())
            temp_ui_data->value_range = QPair<int,int>(defs["weapon_data"]["ranges"]["value"][0].toInt(-1), defs["weapon_data"]["ranges"]["value"][1].toInt(-1));
        else Utility::error("Failed to read integer range data for value input. Default values will be used.");

        if (defs["weapon_data"]["ranges"]["damage"].isArray())
            temp_ui_data->damage_range = QPair<int,int>(defs["weapon_data"]["ranges"]["damage"][0].toInt(-1), defs["weapon_data"]["ranges"]["damage"][1].toInt(-1));
        else Utility::error("Failed to read integer range data for damage input. Default values will be used.");

        if (defs["weapon_data"]["ranges"]["arcane_cost"].isArray())
            temp_ui_data->arcane_cost_range = QPair<int,int>(defs["weapon_data"]["ranges"]["arcane_cost"][0].toInt(-1), defs["weapon_data"]["ranges"]["arcane_cost"][1].toInt(-1));
        else Utility::error("Failed to read integer range data for arcane cost input. Default values will be used.");

        if (defs["weapon_data"]["ranges"]["enchantment_limit"].isArray())
            temp_ui_data->enchantment_limit_range = QPair<int,int>(defs["weapon_data"]["ranges"]["enchantment_limit"][0].toInt(-1), defs["weapon_data"]["ranges"]["enchantment_limit"][1].toInt(-1));
        else Utility::error("Failed to read integer range data for enchantment limit input. Default values will be used.");

    } else Utility::error("Failed to read integer range data. All ranges will be set to default values.");


    // Get rarity slot entries.
    if (!defs["weapon_data"]["field_data"]["rarity"].isArray())
        return fail("External file failed rarity data check! File will be ignored.");

    for (QJsonValue item : defs["weapon_data"]["field_data"]["rarity"].toArray())
        if(!item.isString())
            Utility::error("Bad rarity data. Entry will be skipped.");
        else
            temp_ui_data->rarity_list.append(item.toString());
    //temp_ui_data->rarity_list.sort(); // Rarity list is already sorted in file so no need to sort. Left line here in case I change my mind.


    // Get damage variance entries.
    if (!defs["weapon_data"]["field_data"]["damage_variance"].isArray())
        return fail("External file failed damage variance data check! File will be ignored.");

    for (QJsonValue item : defs["weapon_data"]["field_data"]["damage_variance"].toArray())
        if(!item.isString())
            Utility::error("Bad damage variance data. Entry will be skipped.");
        else
            temp_ui_data->damage_variance_list.append(item.toString());
    //temp_ui_data->damage_variance_list.sort(); // Damage variance list is already sorted in file so no need to sort. Left line here in case I change my mind.


    // Get damage variance entries.
    if (!defs["weapon_data"]["field_data"]["damage_types"].isArray())
        return fail("External file failed damage types data check! File will be ignored.");

    for (QJsonValue item : defs["weapon_data"]["field_data"]["damage_types"].toArray())
        if(!item.isString())
            Utility::error("Bad damage types data. Entry will be skipped.");
        else
            temp_ui_data->damage_types_list.append(item.toString());
    temp_ui_data->damage_types_list.sort();


    // Get spell entries.
    if (!defs["weapon_data"]["field_data"]["spells"].isArray())
        return fail("External file failed damage types data check! File will be ignored.");

    for (QJsonValue item : defs["weapon_data"]["field_data"]["spells"].toArray())
        if(!item.isString())
            Utility::error("Bad spells data. Entry will be skipped.");
        else
            temp_ui_data->spells_list.append(item.toString());
    temp_ui_data->spells_list.sort();


    // Set new data. QSharedPointer should delete discarded data.
    ui_data = temp_ui_data;

    // Setup delegates
    DamageComboBoxDelegate::setup(ui_data->damage_types_list);
    SpellComboBoxDelegate::setup(ui_data->spells_list);

    // Done
    return true;
}

void WeaponWindow::populate_ui()
{
    // This re/populates the ui with data from the ui_data object.
    // This function should only be called after changes are made to the ui_data object (generally via load_defs).
    // You may want to call update_ui after calling this to reload the mod data into the ui as this will reset most fields.

    // Reset child widgets
    tags_widget->reload_ui();
    effects_widget->reload_ui();
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
    set_range(ui->damageSpinBox, ui_data->damage_range);
    set_range(ui->arcaneCostSpinBox, ui_data->arcane_cost_range);
    set_range(ui->effectLimitSpinBox, ui_data->enchantment_limit_range);

    // Setup combo boxes.

    // Simple lambda to save my fingers some wear
    auto setup = [] (QComboBox *combo_box, const QStringList &list) {
        combo_box->blockSignals(true);
        combo_box->clear();
        combo_box->addItems(list);
        combo_box->blockSignals(false);
    };

    setup(ui->rarityComboBox, ui_data->rarity_list);
    setup(ui->varianceComboBox, ui_data->damage_variance_list);
}

void WeaponWindow::save(bool as)
{
    // This handles the save dialog and the basic save logic. Most of the heavy lifting is done in WeaponMod::save_file.

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

void WeaponWindow::set_titles(const QString &title)
{
    // You can figure this one out on your own.

    setWindowTitle(title);
    colours_widget->setWindowTitle(title);
    tags_widget->setWindowTitle(title);
    effects_widget->setWindowTitle(title);
}

void WeaponWindow::update_ui()
{
    // This goes through the TattooMod object (named 'data') and applies all its data to the ui fields.

    // Update spin boxes
    ui->valueSpinBox->setValue(data.value);
    ui->damageSpinBox->setValue(data.damage);
    ui->arcaneCostSpinBox->setValue(data.arcane_cost);
    ui->effectLimitSpinBox->setValue(data.enchantment_limit);


    // Lambda for check boxes. I'm REALLY lazy.
    auto update_toggle = [] (bool state, QCheckBox *box) {
        if (state) // Plural by default toggle
            box->setCheckState(Qt::CheckState::Checked);
        else box->setCheckState(Qt::CheckState::Unchecked);
    };

    update_toggle(data.plural_default, ui->pluralCheckBox);
    update_toggle(data.melee, ui->meleeCheckBox);
    update_toggle(data.two_handed, ui->twoHandedCheckBox);


    // Quick lambda for setting the colour preset checkboxes
    const auto update_colour_toggle = [] (Colour *colour, QCheckBox *toggle) {
        if (colour->getType() == Colour::Type::preset)
            toggle->setCheckState(Qt::CheckState::Checked);
        else toggle->setCheckState(Qt::CheckState::Unchecked);
    };

    update_colour_toggle(data.primary_colour.get(), ui->pColPresetCheckBox);
    update_colour_toggle(data.primary_colour_dye.get(), ui->pColDyePresetCheckBox);

    update_colour_toggle(data.secondary_colour.get(), ui->sColPresetCheckBox);
    update_colour_toggle(data.secondary_colour_dye.get(), ui->sColDyePresetCheckBox);


    // Update combo boxes
    ui->rarityComboBox->setCurrentIndex(ui->rarityComboBox->findText(data.rarity));
    ui->varianceComboBox->setCurrentIndex(ui->varianceComboBox->findText(data.damage_variance));


    // Update line edits
    ui->nameEdit->setText(data.name);
    ui->determinerEdit->setText(data.determiner);
    ui->pluralNameEdit->setText(data.name_plural);
    ui->descriptorEdit->setText(data.attack_descriptor);
    ui->imageNameEdit->setText(data.image_name);
    ui->equippedImageEdit->setText(data.equipped_image_name);


    // Update plain text edits
    ui->descriptionEdit->setPlainText(data.description);
    ui->tooltipTextEdit->setPlainText(data.attack_tooltip);
    ui->equipTextEdit->setPlainText(data.equip_text);
    ui->unequipTextEdit->setPlainText(data.unequip_text);


    // Misc updates
    tags_widget->update();
    effects_list_handler->update(data.effects);
    damage_type_list_handler->update();
    spell_list_handler->update();
    hit_text_list_handler->update();
    miss_text_list_handler->update();
}

WeaponWindow::UiData::UiData()
{
    value_range = QPair<int,int>(-1,-1);
    damage_range = QPair<int,int>(-1,-1);
    arcane_cost_range = QPair<int,int>(-1,-1);
    enchantment_limit_range = QPair<int,int>(-1,-1);
    rarity_list = QStringList();
    damage_variance_list = QStringList();
    damage_types_list = QStringList();
    spells_list = QStringList();
}

QSharedPointer<WeaponWindow::UiData> WeaponWindow::ui_data = QSharedPointer<WeaponWindow::UiData>();
