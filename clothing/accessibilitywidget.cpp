#include "accessibilitywidget.h"
#include "ui_accessibilitywidget.h"

#include <QMenu>
#include <QShortcut>
#include <QStandardItemModel>
#include <QWhatsThis>

#include "common/customcomboboxdelegate.h"

AccessibilityWidget::AccessibilityWidget(QMap<QString, QList<ClothingMod::BlockedParts>> &data, QWidget *parent) : RestrictedSlotEditorBase(parent), ui(new Ui::AccessibilityWidget), map_data(&data)
{
    // Setup
    ui->setupUi(this);
    ui->typeList->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->typeList->setItemDelegate(new CustomComboBoxDelegate(displacement_types, this));
    ui->accessRequiredList->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->accessRequiredList->setItemDelegate(new CustomComboBoxDelegate(access_slots, this));
    ui->blockedBodypartsList->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->blockedBodypartsList->setItemDelegate(new CustomComboBoxDelegate(blocked_bodyparts, this));
    ui->blockedClothingList->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->blockedClothingList->setItemDelegate(new CustomComboBoxDelegate(access_slots, this));
    ui->concealList->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->concealList->setItemDelegate(new CustomComboBoxDelegate(concealed_slots, this));
    ui->typeList->setFocusPolicy(Qt::NoFocus); // Prevents keyboard focus
    hide();

    // Create menu object
    menu = new QMenu(this);

    // Shortcuts
    new QShortcut(QKeySequence("F1"), this, SLOT(whats_this()));

    // Connections
    connect(parent->window(), &QWidget::windowTitleChanged, [this] (const QString &title) {setWindowTitle(title);});
    connect(ui->typeList, &QListView::pressed, [this] (const QModelIndex &index) {show_section(index.row());});
    connect(ui->concealList, &QListView::pressed, [this] (const QModelIndex &index) {set_concealed_preset_data(index);});
    connect(ui->typeList, &QListView::customContextMenuRequested, [this] (const QPoint &pos) {context_menu(pos, Context::displacement_type_menu);});
    connect(ui->accessRequiredList, &QListView::customContextMenuRequested, [this] (const QPoint &pos) {context_menu(pos, Context::access_required_menu);});
    connect(ui->blockedBodypartsList, &QListView::customContextMenuRequested, [this] (const QPoint &pos) {context_menu(pos, Context::blocked_bodyparts_menu);});
    connect(ui->blockedClothingList, &QListView::customContextMenuRequested, [this] (const QPoint &pos) {context_menu(pos, Context::blocked_clothing_access_menu);});
    connect(ui->concealList, &QListView::customContextMenuRequested, [this] (const QPoint &pos) {context_menu(pos, Context::concealed_menu);});
    connect(ui->saveButton, &QPushButton::pressed, [this] () {save_close();});
    connect(ui->discardButton, &QPushButton::pressed, [this] () {this->parentWidget()->window()->show(); this->hide();});
    connect(ui->whatsThisButton, &QPushButton::pressed, [this] () {whats_this();});
    connect(ui->concealSlotButton, &QRadioButton::toggled, [this] (const bool state) {if (state) set_concealed_slots();});
    connect(ui->concealPresetButton, &QRadioButton::toggled, [this] (const bool state) {if (state) set_concealed_preset();});
}

AccessibilityWidget::~AccessibilityWidget()
{
    delete ui;
}

void AccessibilityWidget::open_editor(const QString &slot)
{
    // Load data into ui and show window
    current_slot = slot;
    current_item = map_data->value(slot);

    // Display slot
    ui->slot_display_label->setText("Slot: " + slot);

    // Init typeList
    {
        // Delete old model
        QStandardItemModel *model = qobject_cast<QStandardItemModel*>(ui->typeList->model());
        if (model != nullptr)
            delete model;

        // Create new model and populate it
        model = new QStandardItemModel(ui->typeList);
        for (const ClothingMod::BlockedParts &entry : current_item) {
            model->appendRow(new QStandardItem(entry.displacement_type));
        }

        // Setup model/view
        ui->typeList->setModel(model);
        connect(model, &QStandardItemModel::itemChanged, [this] (QStandardItem *item) {type_list_modify_data(item->index());});
        if (model->rowCount() != 0)
            ui->typeList->setCurrentIndex(model->index(0,0));
    }

    // Init the rest of the ui if there is an item selected
    if (ui->typeList->currentIndex().isValid())
        show_section(ui->typeList->currentIndex().row());
    else {
        // Reset ui if there is no item selected
        if (ui->accessRequiredList->model())
            delete ui->accessRequiredList->model();
        ui->accessRequiredList->setModel(new QStandardItemModel(ui->accessRequiredList));
        if (ui->blockedBodypartsList->model())
            delete ui->blockedBodypartsList->model();
        ui->blockedBodypartsList->setModel(new QStandardItemModel(ui->blockedBodypartsList));
        if (ui->blockedClothingList->model())
            delete ui->blockedClothingList->model();
        ui->blockedClothingList->setModel(new QStandardItemModel(ui->blockedClothingList));
        if (ui->concealList->model())
            delete ui->concealList->model();
        ui->concealList->setModel(new QStandardItemModel(ui->concealList));
        ui->concealSlotButton->setChecked(true);
    }

    this->show();
}

void AccessibilityWidget::setup(const QStringList &displacement_types, const QStringList &access_slots, const QStringList &blocked_bodyparts, const QStringList &concealed_slots, const QStringList &concealed_presets)
{
    // Setup UI data

    AccessibilityWidget::displacement_types = displacement_types;
    AccessibilityWidget::access_slots = access_slots;
    AccessibilityWidget::blocked_bodyparts = blocked_bodyparts;
    AccessibilityWidget::concealed_slots = concealed_slots;
    AccessibilityWidget::concealed_presets = concealed_presets;
}

void AccessibilityWidget::whats_this()
{
    // What do you think this does?
    QWhatsThis::enterWhatsThisMode();
}

void AccessibilityWidget::save_close()
{
    // Apply data and swap windows

    map_data->insert(current_slot, current_item); // Overwrites old data
    this->parentWidget()->window()->show();
    this->hide();
}

void AccessibilityWidget::show_section(int index)
{
    // Updates ui fields to match currently selected section

    // Setup access required list
    if (ui->accessRequiredList->model())
        delete ui->accessRequiredList->model();
    auto model = new QStandardItemModel(ui->accessRequiredList);
    for (const QString &entry : current_item[index].access_required)
        model->appendRow(new QStandardItem(entry));
    connect(model, &QStandardItemModel::itemChanged, [this, index] (QStandardItem *item) {string_list_modify_data(item->index(), current_item[index].access_required);});
    ui->accessRequiredList->setModel(model);

    // Setup blocked bodyparts list
    if (ui->blockedBodypartsList->model())
        delete ui->blockedBodypartsList->model();
    model = new QStandardItemModel(ui->blockedBodypartsList);
    for (const QString &entry : current_item[index].blocked_bodyparts)
        model->appendRow(new QStandardItem(entry));
    connect(model, &QStandardItemModel::itemChanged, [this, index] (QStandardItem *item) {string_list_modify_data(item->index(), current_item[index].blocked_bodyparts);});
    ui->blockedBodypartsList->setModel(model);

    // Setup blocked clothing access list
    if (ui->blockedClothingList->model())
        delete ui->blockedClothingList->model();
    model = new QStandardItemModel(ui->blockedClothingList);
    for (const QString &entry : current_item[index].access_blocked)
        model->appendRow(new QStandardItem(entry));
    connect(model, &QStandardItemModel::itemChanged, [this, index] (QStandardItem *item) {string_list_modify_data(item->index(), current_item[index].access_blocked);});
    ui->blockedClothingList->setModel(model);

    // Setup concealed slot/preset list
    ui->concealSlotButton->blockSignals(true);
    ui->concealPresetButton->blockSignals(true);
    if (current_item[index].concealed_slots->getType() == ClothingMod::BlockedParts::ConcealedSlots::Type::list) {
        // List of slots
        ui->concealSlotButton->setChecked(true);
        set_concealed_slots();
    } else {
        // A preset
        ui->concealPresetButton->setChecked(true);
        set_concealed_preset();
    }
    ui->concealSlotButton->blockSignals(false);
    ui->concealPresetButton->blockSignals(false);
}

void AccessibilityWidget::set_concealed_preset()
{
    // Display presets on UI and set concealed slots data to preset mode

    if (ui->concealList->model())
        delete ui->concealList->model();
    auto model = new QStandardItemModel(ui->concealList);

    for (const QString &preset : concealed_presets)
        model->appendRow(new QStandardItem(preset));
    ui->concealList->setModel(model);

    ui->concealList->clearSelection();
    if (current_item[ui->typeList->currentIndex().row()].concealed_slots->getType() == ClothingMod::BlockedParts::ConcealedSlots::Type::list)
        current_item[ui->typeList->currentIndex().row()].concealed_slots->setPreset("NULL");

    QModelIndexList items = model->match(model->index(0, 0), Qt::DisplayRole, QVariant(current_item[ui->typeList->currentIndex().row()].concealed_slots->getPreset()), 1, Qt::MatchWrap | Qt::MatchFixedString);
    if (!items.isEmpty())
        ui->concealList->setCurrentIndex(items.first());
    else
        ui->concealList->setCurrentIndex(QModelIndex());

    ui->concealList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->concealList->setToolTip(concealed_preset_tooltip);
}

void AccessibilityWidget::set_concealed_slots()
{
    // Display presets on UI and set concealed slots data to slot list mode

    ui->concealList->clearSelection();
    if (ui->concealList->model())
        delete ui->concealList->model();
    auto model = new QStandardItemModel(ui->concealList);

    if (current_item[ui->typeList->currentIndex().row()].concealed_slots->getType() == ClothingMod::BlockedParts::ConcealedSlots::Type::preset) {
        QStringList temp = QStringList();
        current_item[ui->typeList->currentIndex().row()].concealed_slots->setSlots(temp);
    } else {
        for (const QString &entry : current_item[ui->typeList->currentIndex().row()].concealed_slots->getSlots())
            model->appendRow(new QStandardItem(entry));
    }

    connect(model, &QStandardItemModel::itemChanged, [this] (QStandardItem *item) {concealed_list_modify_data(item->index());});
    ui->concealList->setModel(model);

    ui->concealList->setEditTriggers(QAbstractItemView::DoubleClicked);
    ui->concealList->setToolTip(concealed_slots_tooltip);
}

void AccessibilityWidget::set_concealed_preset_data(const QModelIndex &index)
{
    // Sets preset to data if and only if data is in preset mode

    if (current_item[ui->typeList->currentIndex().row()].concealed_slots->getType() == ClothingMod::BlockedParts::ConcealedSlots::Type::preset)
        current_item[ui->typeList->currentIndex().row()].concealed_slots->setPreset(index.data().toString());
}

void AccessibilityWidget::context_menu(const QPoint &pos, AccessibilityWidget::Context flag)
{
    // Context menu. Doesn't show up on concealed list unless it is in slot list mode.

    if (flag != Context::concealed_menu || current_item[ui->typeList->currentIndex().row()].concealed_slots->getType() == ClothingMod::BlockedParts::ConcealedSlots::Type::list) {

        // Setup
        menu->clear();
        QAction *add_action = new QAction("Add entry"), *remove_action = nullptr;
        int index = ui->typeList->currentIndex().row();
        QPoint loc;

        // Setup actions depending on which view is the target
        switch (flag) {
        case Context::displacement_type_menu:
            connect(add_action, &QAction::triggered, [this] () {type_list_add_item();});
            if (ui->typeList->indexAt(pos).isValid()) {
                remove_action = new QAction("Remove entry");
                connect(remove_action, &QAction::triggered, [this, pos] () {type_list_remove_item(ui->typeList->indexAt(pos));});
            }
            loc = ui->typeList->viewport()->mapToGlobal(pos);
            break;
        case Context::access_required_menu:
            connect(add_action, &QAction::triggered, [this, index] () {string_list_add_item(ui->accessRequiredList, current_item[index].access_required);});
            if (ui->accessRequiredList->indexAt(pos).isValid()) {
                remove_action = new QAction("Remove entry");
                connect(remove_action, &QAction::triggered, [this, pos, index] () {string_list_remove_item(ui->accessRequiredList->indexAt(pos), ui->accessRequiredList, current_item[index].access_required);});
            }
            loc = ui->accessRequiredList->viewport()->mapToGlobal(pos);
            break;
        case Context::blocked_bodyparts_menu:
            connect(add_action, &QAction::triggered, [this, index] () {string_list_add_item(ui->blockedBodypartsList, current_item[index].blocked_bodyparts);});
            if (ui->blockedBodypartsList->indexAt(pos).isValid()) {
                remove_action = new QAction("Remove entry");
                connect(remove_action, &QAction::triggered, [this, pos, index] () {string_list_remove_item(ui->blockedBodypartsList->indexAt(pos), ui->blockedBodypartsList, current_item[index].blocked_bodyparts);});
            }
            loc = ui->blockedBodypartsList->viewport()->mapToGlobal(pos);
            break;
        case Context::blocked_clothing_access_menu:
            connect(add_action, &QAction::triggered, [this, index] () {string_list_add_item(ui->blockedClothingList, current_item[index].access_blocked);});
            if (ui->blockedClothingList->indexAt(pos).isValid()) {
                remove_action = new QAction("Remove entry");
                connect(remove_action, &QAction::triggered, [this, pos, index] () {string_list_remove_item(ui->blockedClothingList->indexAt(pos), ui->blockedClothingList, current_item[index].access_blocked);});
            }
            loc = ui->blockedClothingList->viewport()->mapToGlobal(pos);
            break;
        case Context::concealed_menu:
            connect(add_action, &QAction::triggered, [this, index] {concealed_list_add_item(index);});
            if (ui->concealList->indexAt(pos).isValid()) {
                remove_action = new QAction("Remove entry");
                connect(remove_action, &QAction::triggered, [this, pos] () {concealed_list_remove_item(ui->concealList->indexAt(pos));});
            }
            loc = ui->concealList->viewport()->mapToGlobal(pos);
            break;
        }

        // Add actions to menu
        menu->addAction(add_action);
        if (remove_action)
            menu->addAction(remove_action);

        // Show menu
        menu->popup(loc);
    }
}

void AccessibilityWidget::type_list_modify_data(const QModelIndex &index)
{
    // Modify data at index

    if (index.isValid())
        current_item[index.row()].displacement_type = index.data().toString();
}

void AccessibilityWidget::type_list_remove_item(const QModelIndex &index)
{
    // Remove data at index

    if (index.isValid()) {
        // Remove item from ui and data
        current_item.removeAt(index.row());
        ui->typeList->model()->removeRow(index.row());

        // Update ui
        if (!current_item.isEmpty()) {
            // Select first item
            ui->typeList->setCurrentIndex(ui->typeList->model()->index(0, 0));
            show_section(0);
        } else {
            // Reset ui
            if (ui->accessRequiredList->model())
                delete ui->accessRequiredList->model();
            ui->accessRequiredList->setModel(new QStandardItemModel(ui->accessRequiredList));
            if (ui->blockedBodypartsList->model())
                delete ui->blockedBodypartsList->model();
            ui->blockedBodypartsList->setModel(new QStandardItemModel(ui->blockedBodypartsList));
            if (ui->blockedClothingList->model())
                delete ui->blockedClothingList->model();
            ui->blockedClothingList->setModel(new QStandardItemModel(ui->blockedClothingList));
            if (ui->concealList->model())
                delete ui->concealList->model();
            ui->concealList->setModel(new QStandardItemModel(ui->concealList));

            ui->concealSlotButton->blockSignals(true);
            ui->concealPresetButton->blockSignals(true);
            ui->concealSlotButton->setChecked(true);
            ui->concealSlotButton->blockSignals(false);
            ui->concealPresetButton->blockSignals(false);
        }
    }
}

void AccessibilityWidget::type_list_add_item()
{
    // Add new item to data and ui

    // Initialize new object
    ClothingMod::BlockedParts obj = ClothingMod::BlockedParts();
    obj.displacement_type = "NULL";
    obj.access_required = QStringList();
    obj.access_blocked = QStringList();
    obj.blocked_bodyparts = QStringList();

    // Add new object
    current_item.append(obj);
    qobject_cast<QStandardItemModel*>(ui->typeList->model())->appendRow(new QStandardItem(obj.displacement_type));
}

void AccessibilityWidget::string_list_modify_data(const QModelIndex &index, QStringList &list)
{
    // Modify data at index

    if (index.isValid())
        list[index.row()] = index.data().toString();
}

void AccessibilityWidget::string_list_remove_item(const QModelIndex &index, QListView *view, QStringList &list)
{
    // Remove data at index

    if (index.isValid()) {
        list.removeAt(index.row());
        view->model()->removeRow(index.row());
    }
}

void AccessibilityWidget::string_list_add_item(QListView *view, QStringList &list)
{
    // Add new item to data and ui

    list.append(QString());
    qobject_cast<QStandardItemModel*>(view->model())->appendRow(new QStandardItem("NULL"));
}

void AccessibilityWidget::concealed_list_modify_data(const QModelIndex &index)
{
    // Modify data at index if and only if it is the right mode

    if (current_item[ui->typeList->currentIndex().row()].concealed_slots->getType() == ClothingMod::BlockedParts::ConcealedSlots::Type::list) {
        QStringList list = current_item[ui->typeList->currentIndex().row()].concealed_slots->getSlots();
        list[index.row()] = index.data().toString();
        current_item[ui->typeList->currentIndex().row()].concealed_slots->setSlots(list);
    }
}

void AccessibilityWidget::concealed_list_remove_item(const QModelIndex &index)
{
    // Remove data at index

    if (index.isValid()) {
        QStringList list = current_item[ui->typeList->currentIndex().row()].concealed_slots->getSlots();
        list.removeAt(index.row());
        current_item[ui->typeList->currentIndex().row()].concealed_slots->setSlots(list);

        ui->concealList->model()->removeRow(index.row());
    }
}

void AccessibilityWidget::concealed_list_add_item(int section)
{
    // Add new item to data and ui

    QStringList list = current_item[section].concealed_slots->getSlots();
    list.append(QString());
    current_item[section].concealed_slots->setSlots(list);

    qobject_cast<QStandardItemModel*>(ui->concealList->model())->appendRow(new QStandardItem("NULL"));
}

// Tooltips
const QString AccessibilityWidget::concealed_slots_tooltip = "List of slots this displacement mode conceals.";
const QString AccessibilityWidget::concealed_preset_tooltip = "The slot preset that this displacement mode conceals.";

// Initialize static vars
QStringList AccessibilityWidget::displacement_types = QStringList();
QStringList AccessibilityWidget::access_slots = QStringList();
QStringList AccessibilityWidget::blocked_bodyparts = QStringList();
QStringList AccessibilityWidget::concealed_slots = QStringList();
QStringList AccessibilityWidget::concealed_presets = QStringList();
