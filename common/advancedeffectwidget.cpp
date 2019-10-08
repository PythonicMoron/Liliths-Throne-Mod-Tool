#include "advancedeffectwidget.h"
#include "ui_advancedeffectwidget.h"

#include <QStandardItemModel>
#include <QShortcut>
#include <QWhatsThis>
#include <QCloseEvent>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "utility.h"

// Definitions for AdvancedEffectWidget::Effect

// Constructors
AdvancedEffectWidget::Effect::Effect(int type, QList<int> &secondary, QList<int> &potency, QPair<int,int> &limit, QPair<int,int> &timer) :
    limit_range(limit), timer_range(timer), effect_type(type), potency_options(potency), secondary_mods(secondary) {}

AdvancedEffectWidget::Effect::Effect(const AdvancedEffectWidget::Effect &other)
{
    // Copy constructor. Had to define it becuase... I actually don't remember. But I'm sure I had a good reason, so don't delete it.

    limit_range = other.limit_range;
    timer_range = other.timer_range;
    effect_type = other.effect_type;
    potency_options = other.potency_options;
    secondary_mods = other.secondary_mods;
}

AdvancedEffectWidget::Effect::Effect()
{
    limit_range = QPair<int,int>();
    timer_range = QPair<int,int>();
    effect_type = -1;
    potency_options = QList<int>();
    secondary_mods = QList<int>();
}

// Functions
int AdvancedEffectWidget::Effect::potency_index(const QString &str)
{
    // This returns the index of the string in potency_strings or creates one if the string isn't found.

    int index = potency_strings.indexOf(str);
    if (index != -1)
        return index;
    potency_strings.append(str);
    return potency_strings.count() - 1;
}

int AdvancedEffectWidget::Effect::secondary_index(const QString &str)
{
    // This returns the index of the string in secondary_strings or creates one if the string isn't found.

    int index = secondary_strings.indexOf(str);
    if (index != -1)
        return index;
    secondary_strings.append(str);
    return secondary_strings.count() - 1;
}

int AdvancedEffectWidget::Effect::effect_index(const QString &str)
{
    // This returns the index of the string in type_strings or creates one if the string isn't found.

    int index = type_strings.indexOf(str);
    if (index != -1)
        return index;
    type_strings.append(str);
    return type_strings.count() - 1;
}

// Accessors
QStringList AdvancedEffectWidget::Effect::get_secondary_mods()
{
    // Converts the list of indexes to a list of strings.

    QStringList list = QStringList();
    for (int i : secondary_mods)
        list.append(secondary_strings[i]);
    return list;
}

QList<int> AdvancedEffectWidget::Effect::get_secondary_mods_index()
{
    // Returns the list of indexes.
    return secondary_mods;
}

QString AdvancedEffectWidget::Effect::get_effect_type()
{
    // Converts the index to a string and returns it or an empty string if it was invalid.

    if (effect_type < 0 || effect_type >= type_strings.count())
        return QString();
    return type_strings[effect_type];
}

int AdvancedEffectWidget::Effect::get_effect_type_index()
{
    // Return the index
    return effect_type;
}

QStringList AdvancedEffectWidget::Effect::get_potency_options()
{
    // Converts the list of indexes to a list of strings.

    QStringList list = QStringList();
    for (int i : potency_options)
        list.append(potency_strings[i]);
    return list;
}

QList<int> AdvancedEffectWidget::Effect::get_potency_options_index()
{
    // Returns the list of indexes.
    return potency_options;
}

// Static vars
QStringList AdvancedEffectWidget::Effect::potency_strings = QStringList();
QStringList AdvancedEffectWidget::Effect::secondary_strings = QStringList();
QStringList AdvancedEffectWidget::Effect::type_strings = QStringList();

// End definitions for AdvancedEffectWidget::Effect



AdvancedEffectWidget::AdvancedEffectWidget(Mode mode, QList<DataCommon::Effect> *data, QWidget *parent) : QWidget(parent), ui(new Ui::AdvancedEffectWidget), mode(mode), data_list(data)
{
    // Setup
    ui->setupUi(this);
    setWindowFlag(Qt::Window);
    ui->primaryModList->setFocusPolicy(Qt::NoFocus);
    ui->secondaryModList->setFocusPolicy(Qt::NoFocus);
    ui->potencyList->setFocusPolicy(Qt::NoFocus);
    hide();

    // Shortcuts
    new QShortcut(QKeySequence("F1"), this, SLOT(whats_this()));

    // Connections
    connect(parent->window(), &QWidget::windowTitleChanged, [this] (const QString &title) {setWindowTitle(title);});
    connect(ui->primaryModList, &QListView::pressed, [this] (const QModelIndex &index) {current_item->primary_modifier = index.data().toString(); current_item->secondary_modifier = QString(); populate_secondary(); populate_ui();});
    connect(ui->secondaryModList, &QListView::pressed, [this] (const QModelIndex &index) {current_item->secondary_modifier = index.data().toString(); populate_ui();});
    connect(ui->potencyList, &QListView::pressed, [this] (const QModelIndex &index) {current_item->potency = index.data().toString();});
    connect(ui->limitInput, QOverload<int>::of(&QSpinBox::valueChanged), [this] (int value) {current_item->limit = value;});
    connect(ui->timerInput, QOverload<int>::of(&QSpinBox::valueChanged), [this] (int value) {current_item->timer = value;});
    connect(ui->saveButton, &QPushButton::pressed, [this] () {save_close();});
    connect(ui->discardButton, &QPushButton::pressed, [this] () {this->parentWidget()->show(); this->hide();});
    connect(ui->whatsThisButton, &QPushButton::pressed, [this] () {whats_this();});
}

AdvancedEffectWidget::~AdvancedEffectWidget()
{
    // We don't own the data list, so it's not our problem.

    delete ui;
}

void AdvancedEffectWidget::open(int index)
{
    // This does the usual hide the parent and show the widget, but it also loads the passed effect into the ui.

    // Copy data at index to temp item and save index
    current_item = QSharedPointer<DataCommon::Effect>(new DataCommon::Effect(data_list->at(index)));
    data_index = index;

    // Lambda for finding and selecting the flag in the provided list view, if it exists
    auto select_flag = [] (QListView *view, QString &flag) {
        if (flag.isEmpty() || flag == "NULL") { // Check if the flag is null
            view->setCurrentIndex(QModelIndex());
            flag = view->currentIndex().data().toString();
        } else {
            QModelIndexList match = view->model()->match(view->model()->index(0, 0), Qt::DisplayRole, QVariant(flag), 1, Qt::MatchFixedString);
            if (match.isEmpty()) {
                view->setCurrentIndex(QModelIndex());
                flag = QString();
            } else
                view->setCurrentIndex(match.first());
        }
    };

    // Select primary mod
    select_flag(ui->primaryModList, current_item->primary_modifier);

    // Setup secondary mod list and then set the currently selected one
    populate_secondary();
    select_flag(ui->secondaryModList, current_item->secondary_modifier);

    // Setup the rest of the UI and then select potency flag
    populate_ui();
    select_flag(ui->potencyList, current_item->potency);

    // Set limit and timer
    ui->limitInput->setValue(current_item->limit);
    ui->timerInput->setValue(current_item->timer);

    // Hide parent, show widget
    parentWidget()->hide();
    show();
}

void AdvancedEffectWidget::reload_ui()
{
    // Hide widget, show parent, repopulate ui. Simple.

    // Show parent (if needed) and hide widget
    if (!parentWidget()->isVisible())
        parentWidget()->show();
    hide();

    // Reset primary modifier list
    auto model = new QStandardItemModel(ui->primaryModList);
    for (const QString &entry : effect_maps[mode].uniqueKeys()) {
        model->appendRow(new QStandardItem(entry));
    }
    if (ui->primaryModList->model())
        delete ui->primaryModList->model();
    ui->primaryModList->setModel(model);

    // Init new models for list views
    if (ui->secondaryModList->model())
        delete ui->secondaryModList->model();
    ui->secondaryModList->setModel(new QStandardItemModel(ui->secondaryModList));
    if (ui->potencyList->model())
        delete ui->potencyList->model();
    ui->potencyList->setModel(new QStandardItemModel(ui->potencyList));
}

bool AdvancedEffectWidget::load_effects(bool force_internal)
{
    // This function loads the data from the enchantment json files and replaces the static ui data.

    // Returns succuss state (only returns false if both internal and external files failed to load).
    // The force_internal parameter tells the function to ignore external files, if any, when set to true. Defaults to false.

    // Quick assertion to make sure effect_maps is the desired size
    while (effect_maps.count() <AdvancedEffectWidget::Mode::LAST)
        effect_maps.append(QMultiMap<QString, QSharedPointer<Effect>>());

    // Files to load
    auto files = QStringList();
    while (files.count() <AdvancedEffectWidget::Mode::LAST)
        files.append(QString());
    files[AdvancedEffectWidget::Mode::clothing] = "clothing_enchantments.json";
    files[AdvancedEffectWidget::Mode::weapon] = "weapon_enchantments.json";

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

void AdvancedEffectWidget::whats_this()
{
    QWhatsThis::enterWhatsThisMode();
}

void AdvancedEffectWidget::closeEvent(QCloseEvent *event)
{
    // Overrides the close event. Instead of closing the widget, we just hide it and show the parent.

    event->ignore();
    parentWidget()->show();
    hide();
}

void AdvancedEffectWidget::populate_secondary()
{
    // Populates secondary modifier list view based on the primary modifier.

    // Init new model
    if (ui->secondaryModList->model())
        delete ui->secondaryModList->model();

    // Retrieve all secondary mods from the list.
    QStringList secondary = QStringList();
    for (const QSharedPointer<Effect> &effect : effect_maps[mode].values(current_item->primary_modifier))
        secondary.append(effect->get_secondary_mods());

    // Add secondary mods to a model
    auto model = new QStandardItemModel(ui->secondaryModList);
    for (const QString &entry : secondary)
        model->appendRow(new QStandardItem(entry));

    // Add model to list and set no selection
    ui->secondaryModList->setModel(model);
    ui->secondaryModList->setCurrentIndex(QModelIndex());
}

void AdvancedEffectWidget::populate_ui()
{
    // This populates the rest of the ui based on the effect parameters of the secondary modifier.

    // Check to see if we have valid data to search for
    if (current_item->secondary_modifier.isEmpty()) {
        // If there is no secondary mod string, reset the UI

        // Delete old model and create new one
        if (ui->potencyList->model())
            delete ui->potencyList->model();
        ui->potencyList->setModel(new QStandardItemModel(ui->potencyList));

        // Reset spin boxes
        ui->limitInput->setValue(0);
        ui->timerInput->setValue(0);
    } else {
        // Search for the associated effect
        int index = Effect::secondary_index(current_item->secondary_modifier); // Easier to find an integer in a list than a string
        for (QSharedPointer<AdvancedEffectWidget::Effect> &effect : effect_maps[mode].values(current_item->primary_modifier)) {
            if (effect->get_secondary_mods_index().contains(index)) { // Is this our associated effect?
                // Effect found

                // First, setup potency list

                // Delete old model and create new one
                if (ui->potencyList->model())
                    delete ui->potencyList->model();
                auto model = new QStandardItemModel(ui->potencyList);

                // Populate model with data
                for (const QString &entry : effect->get_potency_options())
                    model->appendRow(new QStandardItem(entry));

                // Apply
                ui->potencyList->setModel(model);
                ui->potencyList->setCurrentIndex(QModelIndex());

                // Done. Now, setup spin box ranges

                // Limit
                ui->limitInput->setRange(effect->limit_range.first, effect->limit_range.second);

                // Timer
                ui->timerInput->setRange(effect->timer_range.first, effect->timer_range.second);

                // Done. Stop iteration
                break;
            }
        }
    }
}

void AdvancedEffectWidget::save_close()
{
    // Apply changes then hide widget and show parent.

    data_list->replace(data_index, *current_item);
    emit data_changed();
    parentWidget()->show();
    hide();
}

// I give you a really fucking long static variable declaration.
QList<QMultiMap<QString,QSharedPointer<AdvancedEffectWidget::Effect>>> AdvancedEffectWidget::effect_maps = QList<QMultiMap<QString,QSharedPointer<AdvancedEffectWidget::Effect>>>();
