#include "enchantmentwidget.h"
#include "ui_enchantmentwidget.h"

#include <QShortcut>
#include <QWhatsThis>
#include <QCloseEvent>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "utility.h"

EnchantmentWidget::EnchantmentWidget(Mode mode, QWidget *parent) : QWidget(parent), ui(new Ui::EnchantmentWidget), mode(mode)
{
    // Setup
    ui->setupUi(this);
    setWindowFlag(Qt::Window);
    hide();

    // Shortcuts
    new QShortcut(QKeySequence("F1"), this, SLOT(whats_this()));

    // Init pointer.
    effect_ptr = nullptr;

    // Make some connections.
    connect(ui->primaryComboBox, QOverload<const QString &>::of(&QComboBox::currentIndexChanged), [this] (const QString &text) {populate_secondary(text);});
    connect(ui->secondaryComboBox, QOverload<const QString &>::of(&QComboBox::currentIndexChanged), [this] (const QString &text) {populate_ui(text);});
    connect(ui->closeButton, &QPushButton::released, [this] () {finish();});
}

EnchantmentWidget::~EnchantmentWidget()
{
    delete ui;
}

void EnchantmentWidget::open(DataCommon::Effect *effect)
{
    // This does the usual hide the parent and show the widget, but it also loads the passed effect into the ui which is a bitch to do.

    // Pointer nonsense.
    effect_ptr = effect;

    // Get the list of smart pointers to effects associated with the primary modifier from the list of maps of smart pointers to effects associated with the current mode. Nope, I'm not kidding.
    QList<QSharedPointer<Effect>> effects_list;
    if (effect->primary_modifier.isNull() || effect->primary_modifier.isEmpty())
        effects_list = effect_maps[mode].values(effect_maps[mode].uniqueKeys().first());
    else
        effects_list = effect_maps[mode].values(effect->primary_modifier);

    // Ensure we have a valid list
    if (effects_list.count() < 1 || effects_list.first()->get_effect_type() == -1)
        return;

    // Setup ui. Manually calling functions since signals seem to be fucky here.
    ui->primaryComboBox->blockSignals(true);
    ui->primaryComboBox->setCurrentIndex(ui->primaryComboBox->findText(effect->primary_modifier));
    ui->primaryComboBox->blockSignals(false);

    populate_secondary(effect->primary_modifier);

    ui->secondaryComboBox->blockSignals(true);
    ui->secondaryComboBox->setCurrentIndex(ui->secondaryComboBox->findText(effect->secondary_modifier));
    ui->secondaryComboBox->blockSignals(false);

    populate_ui(effect->secondary_modifier);

    // Display
    parentWidget()->hide();
    show();
}

void EnchantmentWidget::reload_ui()
{
    // Hide widget, show parent, repopulate ui. Simple.

    // Show parent (if needed) and hide widget
    if (!parentWidget()->isVisible())
        parentWidget()->show();
    hide();

    // Always use protection. You may end up with an unhappy life if you don't.
    ui->primaryComboBox->blockSignals(true);
    ui->secondaryComboBox->blockSignals(true);

    ui->primaryComboBox->clear();
    ui->primaryComboBox->addItems(effect_maps[mode].uniqueKeys());
    ui->primaryComboBox->model()->sort(0);

    // End protected state
    ui->primaryComboBox->blockSignals(false);
    ui->secondaryComboBox->blockSignals(false);
}

bool EnchantmentWidget::load_effects(bool force_internal)
{
    // This function loads the data from the enchantment json files and replaces the static ui data.

    // Returns succuss state (only returns false if both internal and external files failed to load).
    // The force_internal parameter tells the function to ignore external files, if any, when set to true. Defaults to false.

    // Quick assertion to make sure effect_maps is the desired size
    while (effect_maps.count() < EnchantmentWidget::Mode::LAST)
        effect_maps.append(QMultiMap<QString, QSharedPointer<Effect>>());

    // Files to load
    auto files = QStringList();
    while (files.count() < EnchantmentWidget::Mode::LAST)
        files.append(QString());
    files[EnchantmentWidget::Mode::clothing] = "clothing_enchantments.json";
    files[EnchantmentWidget::Mode::weapon] = "weapon_enchantments.json";

    // Load all files
    for (int i = 0; i < files.count(); i++) {

        QFile file(files[i]); // First, check for external file. Allows for overriding internal resource.
        if (!file.exists() || force_internal) {
            file.setFileName(":/res/data_files/" + files[i]); // If no external file exists or this was called with force_internal set to true, load internal file.
            if (!file.exists()) {
                Utility::error("Could not open a valid " + files[i] + " file!"); // Something or someone really fucked up.
                return false;
            }
        }

        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QString data = file.readAll();
        file.close();
        QJsonDocument effects = QJsonDocument::fromJson(data.toUtf8());

        // Lambda to simplify critical errors
        auto fail = [force_internal] (const QString &fail_message) {
            if (force_internal) // This check helps prevent infinite looping during development or in the event something goes really, really wrong.
                return false;
            Utility::error(fail_message);
            return load_effects(true); // Try again with force internal true
        };

        // The internal file is compiled in and unable to be edited, so it should always be valid outside of development testing. Thus, it is assumed that all errors from here on are the fault of an external file.
        if (effects.isNull() || !effects.isObject() || !effects["default"].isObject() || !effects["enchantments"].isArray())
            return fail("External " + files[i] + " file failed structure check! File will be ignored.");

        // Default object is referenced if objects under 'enchantments' are missing values.
        QJsonObject def = effects["default"].toObject();

        // Performs a structure check on default object and if all is good, constructs said object.
        Effect default_effect;
        if (def["effect type"].isUndefined() || def["limit range"].isUndefined() || def["potency options"].isUndefined() || def["timer range"].isUndefined())
            return fail("External " + files[i] + " file is missing default values! File will be ignored.");

        if (!def["effect type"].isString() || !def["limit range"].isArray() || !def["timer range"].isArray() || !def["potency options"].isArray())
            return fail("External " + files[i] + " file has bad default structure! File will be ignored.");
        {

            // Get type index
            int type = Effect::effect_index(def["effect type"].toString());

            // Get limit min and max
            QJsonArray range = def["limit range"].toArray();
            QPair<int,int> limit = QPair<int,int>(range[0].toInt(0), range[1].toInt(0));

            // Get timer min and max
            range = def["timer range"].toArray();
            QPair<int,int> timer = QPair<int,int>(range[0].toInt(0), range[1].toInt(0));

            // Get potency index list
            QList<int> potency = QList<int>();
            for (QJsonValue entry : def["potency options"].toArray()) {
                if (!entry.isString())
                    Utility::error(files[i] + ": bad default potency value! Value will be ignored.");
                else {
                    potency << Effect::potency_index(entry.toString());
                }
            }

            // Build object
            QList<int> secondary = QList<int>();
            default_effect = Effect(type, secondary, potency, limit, timer);
        }


        // Make a QMultiMap object
        auto map = QMultiMap<QString, QSharedPointer<Effect>>();

        // Begin setting up data.
        for (QJsonValue obj : effects["enchantments"].toArray()) {

            // Assert that "primary mod" and "secondary options" must be defined.
            if (!obj.isObject() || obj["primary mod"].isUndefined() || obj["secondary options"].isUndefined()) {
                Utility::error("Bad enchantment data in " + files[i] + "! Effect will be skipped.");
                continue; // Next obj in effects["enchantments"]
            }

            // Get primary modifier
            if (!obj["primary mod"].isString()) {
                Utility::error("Bad primary mod data in " + files[i] + "! Effect will be skipped.");
                continue; // Next obj in effects["enchantments"]
            }
            QString primary_mod = obj["primary mod"].toString();

            // Get secondary modifiers
            if (!obj["secondary options"].isArray()) {
                Utility::error("Bad secondary options data in " + files[i] + "! Effect will be skipped.");
                continue; // Next obj in effects["enchantments"]
            }
            QList<int> secondary_mod = QList<int>();
            for (QJsonValue entry : obj["secondary options"].toArray()) {
                if (!entry.isString()) {
                    Utility::error("Bad secondary modifier string in " + files[i] + "! Entry will be skipped.");
                    continue; // Next entry in obj["secondary options"]
                }
                secondary_mod.append(Effect::secondary_index(entry.toString()));
            }

            // Check if effect type is defined. Use default if not.
            int type = default_effect.get_effect_type_index();
            if (!obj["effect type"].isUndefined()) {
                if (!obj["effect type"].isString()) {
                    Utility::error("Bad effect type data in " + files[i] + "! Effect will be skipped.");
                    continue; // Next obj in effects["enchantments"]
                }
                type = Effect::effect_index(obj["effect type"].toString());
            }

            // Check if limit range is defined. Use default if not.
            QPair<int,int> limit = default_effect.limit_range;
            if (!obj["limit range"].isUndefined()) {
                if (!obj["limit range"].isArray()) {
                    Utility::error("Bad limit range data in " + files[i] + "! Effect will be skipped.");
                    continue; // Next obj in effects["enchantments"]
                }
                QJsonArray range = obj["limit range"].toArray();
                limit = QPair<int,int>(range[0].toInt(limit.first), range[1].toInt(limit.second));
            }

            // Check if timer range is defined. Use default if not.
            QPair<int,int> timer = default_effect.timer_range;
            if (!obj["timer range"].isUndefined()) {
                if (!obj["timer range"].isArray()) {
                    Utility::error("Bad timer range data in " + files[i] + "! Effect will be skipped.");
                    continue; // Next obj in effects["enchantments"]
                }
                QJsonArray range = obj["timer range"].toArray();
                timer = QPair<int,int>(range[0].toInt(limit.first), range[1].toInt(limit.second));
            }

            // Check if potency options are defined. Use default if not.
            QList<int> potency = default_effect.get_potency_options_index();
            if (!obj["potency options"].isUndefined()) {
                if (!obj["potency options"].isArray()) {
                    Utility::error("Bad potency data in " + files[i] + "! Effect will be skipped.");
                    continue; // Next obj in effects["enchantments"]
                }
                potency.clear();
                for (QJsonValue entry : obj["potency options"].toArray()) {
                    if (!entry.isString()) {
                        Utility::error("Bad potency string in " + files[i] + "! Entry will be skipped.");
                        continue; // Next entry in obj["potency options"]
                    }
                    potency.append(Effect::potency_index(entry.toString()));
                }
            }

            // Populate effects map
            map.insert(primary_mod, QSharedPointer<Effect>(new Effect(type, secondary_mod, potency, limit, timer)));
        }

        // If all went well then free up the previous map memory
        effect_maps[i].clear();

        // Add new map to list
        effect_maps[i] = map;
    }

    return true;
}

void EnchantmentWidget::whats_this()
{
    QWhatsThis::enterWhatsThisMode();
}

void EnchantmentWidget::closeEvent(QCloseEvent *event)
{
    // Overrides the close event. Instead of closing the widget, we just hide it and show the parent.

    event->ignore();
    parentWidget()->show();
    hide();
}

void EnchantmentWidget::populate_secondary(const QString &primary)
{
    // Populates secondary modifier combo box based on primary modifier.

    // Get the list of effects from the list of maps based on the primary modifier.
    QList<QSharedPointer<Effect>> effects_list = effect_maps[mode].values(primary);

    // Retrieve all secondary mods from the list.
    QStringList secondary = QStringList();
    for (const QSharedPointer<Effect> &effect : effects_list)
        secondary.append(effect->get_secondary_mods());

    // Apply new items to combo box.
    ui->secondaryComboBox->blockSignals(true);
    ui->secondaryComboBox->clear();
    ui->secondaryComboBox->addItems(secondary);
    ui->secondaryComboBox->blockSignals(false);

    // If we have an empty combo box that has more than 0 entries, select the first. Then, call (or trigger the signal for) populate_ui
    if (ui->secondaryComboBox->currentText().isEmpty() && ui->secondaryComboBox->count() > 0)
        ui->secondaryComboBox->setCurrentIndex(0);
    else
        populate_ui(ui->secondaryComboBox->currentText());
}

void EnchantmentWidget::populate_ui(const QString &secondary)
{
    // This populates the rest of the ui based on the effect parameters of the secondary modifier.

    // Make sure we didn't fuck up.
    if (secondary.isNull() || secondary.isEmpty())
        return;

    // Get the list of shit again
    QList<QSharedPointer<Effect>> effects_list = effect_maps[mode].values(ui->primaryComboBox->currentText());
    int index = Effect::secondary_index(secondary);

    // Find our particular effect and apply its values to the ui.
    for (const QSharedPointer<Effect> &effect : effects_list)
        if (effect->get_secondary_mods_index().contains(index)) {
            // Populate and set type combo box
            ui->typeComboBox->clear();
            ui->typeComboBox->addItem(effect->get_effect_type());
            ui->typeComboBox->setCurrentIndex(ui->typeComboBox->findText(effect->get_effect_type()));
            if (ui->typeComboBox->currentText().isEmpty() && ui->typeComboBox->count() > 0)
                ui->typeComboBox->setCurrentIndex(0);

            // Populate and set potency options combo box
            ui->potencyComboBox->clear();
            ui->potencyComboBox->addItems(effect->get_potency_options());
            ui->potencyComboBox->setCurrentIndex(ui->potencyComboBox->findText(effect_ptr->potency));
            if (ui->potencyComboBox->currentText().isEmpty() && ui->potencyComboBox->count() > 0)
                ui->potencyComboBox->setCurrentIndex(0);

            // Set ranges
            ui->limitSpinBox->setRange(effect->limit_range.first, effect->limit_range.second);
            ui->timerSpinBox->setRange(effect->timer_range.first, effect->timer_range.second);

            break;
        }
}

void EnchantmentWidget::finish()
{
    // This applies the effect to the object our effect pointer is pointing at then emits finished and swaps the widget visibility with the parent window.

    effect_ptr->effect_type = ui->typeComboBox->currentText();
    effect_ptr->primary_modifier = ui->primaryComboBox->currentText();
    effect_ptr->secondary_modifier = ui->secondaryComboBox->currentText();
    effect_ptr->potency = ui->potencyComboBox->currentText();
    effect_ptr->timer = ui->timerSpinBox->value();
    effect_ptr->limit = ui->limitSpinBox->value();

    emit finished();
    parentWidget()->show();
    hide();
}

// I give you a really fucking long static variable declaration. This is AFTER I shortened it.
QList<QMultiMap<QString,QSharedPointer<EnchantmentWidget::Effect>>> EnchantmentWidget::effect_maps = QList<QMultiMap<QString,QSharedPointer<EnchantmentWidget::Effect>>>();


// Definitions for EnchantmentWidget::Effect

// Constructors
EnchantmentWidget::Effect::Effect(int type, QList<int> &secondary, QList<int> &potency, QPair<int,int> &limit, QPair<int,int> &timer) :
    limit_range(limit), timer_range(timer), effect_type(type), potency_options(potency), secondary_mods(secondary) {}

EnchantmentWidget::Effect::Effect(const EnchantmentWidget::Effect &other)
{
    // Copy constructor. Had to define it becuase... I actually don't remember. But I'm sure I had a good reason, so don't delete it.

    limit_range = other.limit_range;
    timer_range = other.timer_range;
    effect_type = other.effect_type;
    potency_options = other.potency_options;
    secondary_mods = other.secondary_mods;
}

EnchantmentWidget::Effect::Effect()
{
    limit_range = QPair<int,int>();
    timer_range = QPair<int,int>();
    effect_type = -1;
    potency_options = QList<int>();
    secondary_mods = QList<int>();
}

// Functions
int EnchantmentWidget::Effect::potency_index(const QString &str)
{
    // This returns the index of the string in potency_strings or creates one if the string isn't found.

    int index = potency_strings.indexOf(str);
    if (index != -1)
        return index;
    potency_strings.append(str);
    return potency_strings.count() - 1;
}

int EnchantmentWidget::Effect::secondary_index(const QString &str)
{
    // This returns the index of the string in secondary_strings or creates one if the string isn't found.

    int index = secondary_strings.indexOf(str);
    if (index != -1)
        return index;
    secondary_strings.append(str);
    return secondary_strings.count() - 1;
}

int EnchantmentWidget::Effect::effect_index(const QString &str)
{
    // This returns the index of the string in type_strings or creates one if the string isn't found.

    int index = type_strings.indexOf(str);
    if (index != -1)
        return index;
    type_strings.append(str);
    return type_strings.count() - 1;
}

// Accessors
QStringList EnchantmentWidget::Effect::get_secondary_mods()
{
    // Converts the list of indexes to a list of strings.

    QStringList list = QStringList();
    for (int i : secondary_mods)
        list.append(secondary_strings[i]);
    return list;
}

QList<int> EnchantmentWidget::Effect::get_secondary_mods_index()
{
    // Returns the list of indexes.
    return secondary_mods;
}

QString EnchantmentWidget::Effect::get_effect_type()
{
    // Converts the index to a string and returns it or an empty string if it was invalid.

    if (effect_type < 0 || effect_type >= type_strings.count())
        return QString();
    return type_strings[effect_type];
}

int EnchantmentWidget::Effect::get_effect_type_index()
{
    // Return the index
    return effect_type;
}

QStringList EnchantmentWidget::Effect::get_potency_options()
{
    // Converts the list of indexes to a list of strings.

    QStringList list = QStringList();
    for (int i : potency_options)
        list.append(potency_strings[i]);
    return list;
}

QList<int> EnchantmentWidget::Effect::get_potency_options_index()
{
    // Returns the list of indexes.
    return potency_options;
}

// Static vars
QStringList EnchantmentWidget::Effect::potency_strings = QStringList();
QStringList EnchantmentWidget::Effect::secondary_strings = QStringList();
QStringList EnchantmentWidget::Effect::type_strings = QStringList();

// End definitions for EnchantmentWidget::Effect
