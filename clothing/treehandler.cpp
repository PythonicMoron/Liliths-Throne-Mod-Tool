#include "treehandler.h"

#include <QMenu>

#include "clothing/treecomboboxdelegate.h"

TreeHandler::TreeHandler(QTreeView *view, QList<ClothingMod::BlockedParts> &data) : QObject(view), tree(view), data(&data)
{
    tree->setToolTip(TreeHandler::widget_tooltip);

    // Setup delegate
    auto *delegate = new TreeComboBoxDelegate(tree);
    tree->setItemDelegate(delegate);
    tree->setContextMenuPolicy(Qt::CustomContextMenu);

    // Create QMenu
    menu = new QMenu();
}

TreeHandler::~TreeHandler()
{
    // The QTreeView and the data objects don't belong to us, so DO NOT DELETE THEM. You wouldn't like it either if I borrowed your stuff and deleted it when I was done with it.

    delete menu;
}

void TreeHandler::update()
{
    // This is called when the tree needs to be generated from the internal data (opposed to user input). Called via ClothingWindow::update_ui

    // We need a new model. Make a new model. If there was an existing model, get rid of it. We need a new model.
    if (tree->model() != nullptr)
        delete tree->model();

    // Setup model
    auto *model = new QStandardItemModel(tree);
    model->setHorizontalHeaderItem(0, new QStandardItem("Blocked Parts List"));
    connect(model, &QStandardItemModel::itemChanged, [this] (QStandardItem *item) {modify_data(item->index());});

    // Iterate through data.blocked_parts array
    for (int i = 0; i < data->count(); i++) {
        // Parent object. Displays as number in array
        QStandardItem *parent = create_item(QString::number(i), true, TreeHandler::widget_tooltip);

        // Create displacement type item
        QStandardItem *item = create_item("Displacement Type", true, TreeHandler::displacement_type_tooltip);
        item->appendRow(create_item((*data)[i].displacement_type, false, TreeHandler::displacement_type_tooltip));
        parent->appendRow(item);

        // Create and populate access required item
        item = create_item("Access Required", true, TreeHandler::access_required_tooltip);
        for (const QString& entry : (*data)[i].access_required)
            item->appendRow(create_item(entry, false, TreeHandler::access_required_tooltip));
        parent->appendRow(item);

        // Create and populate blocked bodyparts item
        item = create_item("Blocked Bodyparts", true, TreeHandler::blocked_bodyparts_tooltip);
        for (const QString& entry : (*data)[i].blocked_bodyparts)
            item->appendRow(create_item(entry, false, TreeHandler::blocked_bodyparts_tooltip));
        parent->appendRow(item);

        // Create and populate access blocked item
        item = create_item("Access Blocked", true, TreeHandler::access_blocked_tooltip);
        for (const QString& entry : (*data)[i].access_blocked)
            item->appendRow(create_item(entry, false, TreeHandler::access_blocked_tooltip));
        parent->appendRow(item);

        // Either create a concealed preset item or create and populate a concealed slots item
        if ((*data)[i].concealed_slots->getType() == ClothingMod::BlockedParts::ConcealedSlots::Type::preset) {
            item = create_item("Concealed Preset", true, TreeHandler::concealed_preset_tooltip);
            item->appendRow(create_item((*data)[i].concealed_slots->getPreset(), false, TreeHandler::concealed_preset_tooltip));
        } else {
            item = create_item("Concealed Slots", true, TreeHandler::concealed_slots_tooltip);
            for (const QString& entry : (*data)[i].concealed_slots->getSlots())
                item->appendRow(create_item(entry, false, TreeHandler::concealed_slots_tooltip));
        }
        parent->appendRow(item);

        // Add parent to model
        model->appendRow(parent);
    }
    tree->setModel(model);
    tree->expandAll();
}

void TreeHandler::contex_menu(const QPoint &pos)
{
    // Displays a complex menu. I mean a context menu.
    // The menu contents differ based on what section/item is selected. Fuck.

    // Empty the menu.
    menu->clear();

    // Get the model index using what I assume is dark sorcery.
    QModelIndex index = tree->indexAt(pos);

    // We need the parent object text and the current object text.
    QString parent = index.parent().data().toString(), text = index.data().toString();

    // Top level item (an entire blockedParts item) or nothing selected
    if (index.isValid() && !index.parent().isValid()) {
        QAction *new_entry = new QAction("New section");
        QAction *rem_entry = new QAction("Remove section");

        connect(new_entry, &QAction::triggered, [this] () {add_section();});
        connect(rem_entry, &QAction::triggered, [this, index] () {remove_section(index.row());});

        menu->addAction(new_entry);
        menu->addAction(rem_entry);
    }
    if (!index.isValid()) {
        QAction *new_entry = new QAction("New section");

        connect(new_entry, &QAction::triggered, [this] () {add_section();});

        menu->addAction(new_entry);
    }

    // Access required section selected
    if (parent == "Access Required") {
        QAction *new_entry = new QAction("New entry", menu);
        QAction *rem_entry = new QAction("Remove entry", menu);

        connect(new_entry, &QAction::triggered, [this, index] () {add_entry(index.parent(), Mode::access_required);});
        connect(rem_entry, &QAction::triggered, [this, index] () {remove_entry(index, Mode::access_required);});

        menu->addAction(new_entry);
        menu->addAction(rem_entry);
    }
    if (text == "Access Required") {
        QAction *new_entry = new QAction("New entry", menu);

        connect(new_entry, &QAction::triggered, [this, index] () {add_entry(index, Mode::access_required);});

        menu->addAction(new_entry);
    }

    // Blocked bodyparts section selected
    if (parent == "Blocked Bodyparts") {
        QAction *new_entry = new QAction("New entry", menu);
        QAction *rem_entry = new QAction("Remove entry", menu);

        connect(new_entry, &QAction::triggered, [this, index] () {add_entry(index.parent(), Mode::blocked_bodyparts);});
        connect(rem_entry, &QAction::triggered, [this, index] () {remove_entry(index, Mode::blocked_bodyparts);});

        menu->addAction(new_entry);
        menu->addAction(rem_entry);
    }
    if (text == "Blocked Bodyparts") {
        QAction *new_entry = new QAction("New entry", menu);

        connect(new_entry, &QAction::triggered, [this, index] () {add_entry(index, Mode::blocked_bodyparts);});

        menu->addAction(new_entry);
    }

    // Access blocked section selected
    if (parent == "Access Blocked") {
        QAction *new_entry = new QAction("New entry", menu);
        QAction *rem_entry = new QAction("Remove entry", menu);

        connect(new_entry, &QAction::triggered, [this, index] () {add_entry(index.parent(), Mode::access_blocked);});
        connect(rem_entry, &QAction::triggered, [this, index] () {remove_entry(index, Mode::access_blocked);});

        menu->addAction(new_entry);
        menu->addAction(rem_entry);
    }
    if (text == "Access Blocked") {
        QAction *new_entry = new QAction("New entry", menu);

        connect(new_entry, &QAction::triggered, [this, index] () {add_entry(index, Mode::access_blocked);});

        menu->addAction(new_entry);
    }

    // Concealed slots section selected
    if (parent == "Concealed Slots") {
        QAction *new_entry = new QAction("New entry", menu);
        QAction *rem_entry = new QAction("Remove entry", menu);

        connect(new_entry, &QAction::triggered, [this, index] () {add_entry(index.parent(), Mode::concealed_slots);});
        connect(rem_entry, &QAction::triggered, [this, index] () {remove_entry(index, Mode::concealed_slots);});

        menu->addAction(new_entry);
        menu->addAction(rem_entry);
    }
    if (text == "Concealed Slots") {
        QAction *new_entry = new QAction("New entry", menu);
        QAction *make_preset = new QAction("Switch to preset", menu);

        connect(new_entry, &QAction::triggered, [this, index] () {add_entry(index, Mode::concealed_slots);});
        connect(make_preset, &QAction::triggered, [this, index] () {set_concealed(index, true);});

        menu->addAction(new_entry);
        menu->addAction(make_preset);
    }

    // Concealed preset section selected
    if (text == "Concealed Preset") {
        QAction *make_preset = new QAction("Switch to slots", menu);

        connect(make_preset, &QAction::triggered, [this, index] () {set_concealed(index, false);});

        menu->addAction(make_preset);
    }

    // Display menu
    menu->popup(tree->viewport()->mapToGlobal(pos));
}

QStandardItem *TreeHandler::create_item(const QString &title, bool is_parent, const QString &tooltip)
{
    // This creates an item with different flags depending on whether or not the item needs to be a parent item.

    // Just to make sure we don't get blank fields.
    QStandardItem *item;
    if (title.isEmpty())
        item = new QStandardItem("NULL");
    else
        item = new QStandardItem(title);

    // Set flags
    if (is_parent)
        item->setFlags(Qt::ItemFlag::ItemIsEnabled | Qt::ItemFlag::ItemIsSelectable);
    else
        item->setFlags(Qt::ItemFlag::ItemIsEditable | Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemIsEnabled | Qt::ItemFlag::ItemNeverHasChildren);

    // Set tooltip
    if (!tooltip.isNull() && !tooltip.isEmpty())
        item->setToolTip(tooltip);

    // Done. Return the item.
    return item;
}

void TreeHandler::modify_data(const QModelIndex &index)
{
    // This modifies the data when a user interacts with the tree.

    // Validity check
    if (!index.parent().isValid() || !index.parent().parent().isValid())
        return;

    // Parent text
    QString parent = index.parent().data().toString();

    // What data is modified is based on parent text, what row the entire BlockedParts section is in, and what row the selected item is in.
    // If this confuses you, don't worry, I'm pretty sure I spent more time on these 15 lines than the other 300+ combined.
    if (parent == "Displacement Type") {
        (*data)[index.parent().parent().row()].displacement_type = index.data().toString();
    } else if (parent == "Access Required") {
        (*data)[index.parent().parent().row()].access_required[index.row()] = index.data().toString();
    } else if (parent == "Blocked Bodyparts") {
        (*data)[index.parent().parent().row()].blocked_bodyparts[index.row()] = index.data().toString();
    } else if (parent == "Access Blocked") {
        (*data)[index.parent().parent().row()].access_blocked[index.row()] = index.data().toString();
    } else if (parent == "Concealed Slots") {
        QStringList list = (*data)[index.parent().parent().row()].concealed_slots->getSlots();
        list[index.row()] = index.data().toString();
        (*data)[index.parent().parent().row()].concealed_slots->setSlots(list);
    } else if (parent == "Concealed Preset") {
        (*data)[index.parent().parent().row()].concealed_slots->setPreset(index.data().toString());
    }
}

void TreeHandler::add_entry(const QModelIndex &index, Mode mode)
{
    // Adds an entry

    const QString *tooltip = nullptr;

    // Add entry in ClothingMod data
    switch (mode){
        case access_required:
            (*data)[index.parent().row()].access_required.append(QString());
            tooltip = &TreeHandler::access_required_tooltip;
            break;
        case blocked_bodyparts:
            (*data)[index.parent().row()].blocked_bodyparts.append(QString());
            tooltip = &TreeHandler::blocked_bodyparts_tooltip;
            break;
        case access_blocked:
            (*data)[index.parent().row()].access_blocked.append(QString());
            tooltip = &TreeHandler::access_blocked_tooltip;
            break;
        case concealed_slots: {
            QStringList list = (*data)[index.parent().row()].concealed_slots->getSlots();
            list.append(QString());
            (*data)[index.parent().row()].concealed_slots->setSlots(list);
            tooltip = &TreeHandler::concealed_slots_tooltip;
            break;
        }
    }

    // Add entry in model
    qobject_cast<QStandardItemModel*>(tree->model())->itemFromIndex(index)->appendRow(create_item("NULL", false, *tooltip));
}

void TreeHandler::remove_entry(const QModelIndex &index, TreeHandler::Mode mode)
{
    // Remove entry

    // We need the parent index too.
    QModelIndex parent = index.parent();

    // Remove data from model.
    tree->model()->removeRow(index.row(), parent);

    // Remove data from ClothingMod data.
    switch (mode){
        case access_required:
            (*data)[parent.parent().row()].access_required.removeAt(index.row());
            break;
        case blocked_bodyparts:
            (*data)[parent.parent().row()].blocked_bodyparts.removeAt(index.row());
            break;
        case access_blocked:
            (*data)[parent.parent().row()].access_blocked.removeAt(index.row());
            break;
        case concealed_slots: {
            QStringList list = (*data)[parent.parent().row()].concealed_slots->getSlots();
            list.removeAt(index.row());
            (*data)[parent.parent().row()].concealed_slots->setSlots(list);
            break;
        }
    }
}

void TreeHandler::set_concealed(const QModelIndex &index, bool preset)
{
    // Switch a concealed section between preset and list mode.

    // We need the whole model. We've got serious work to do.
    auto *model = qobject_cast<QStandardItemModel*>(tree->model());

    // Get the item and remove all of its children. Rough, I know.
    QStandardItem *item = model->itemFromIndex(index);
    model->removeRows(0, item->rowCount(), index);

    // Perform the operation.
    if (preset) {
        item->setText("Concealed Preset");
        item->setToolTip(TreeHandler::concealed_preset_tooltip);
        if ((*data)[index.parent().row()].concealed_slots.isNull())
            (*data)[index.parent().row()].concealed_slots = QSharedPointer<ClothingMod::BlockedParts::ConcealedSlots>(new ClothingMod::BlockedParts::ConcealedSlots(QString()));
        else
            (*data)[index.parent().row()].concealed_slots->setPreset(QString());
        item->appendRow(create_item("NULL", false, TreeHandler::concealed_preset_tooltip));
    } else {
        item->setText("Concealed Slots");
        item->setToolTip(TreeHandler::concealed_slots_tooltip);
        QStringList list = QStringList();
        if ((*data)[index.parent().row()].concealed_slots.isNull())
            (*data)[index.parent().row()].concealed_slots = QSharedPointer<ClothingMod::BlockedParts::ConcealedSlots>(new ClothingMod::BlockedParts::ConcealedSlots(QStringList()));
        else
            (*data)[index.parent().row()].concealed_slots->setSlots(list);
    }
}

void TreeHandler::add_section()
{
    // This creates an entire new BlockedParts item.

    // Doing this for the data is easy. The model, not so much.
    data->append(ClothingMod::BlockedParts());

    // Parent object. Displays as number in array
    QStandardItem *parent = create_item(QString::number(tree->model()->rowCount()), true, TreeHandler::widget_tooltip);

    // Create displacement type item with default entry
    QStandardItem *item = create_item("Displacement Type", true, TreeHandler::displacement_type_tooltip);
    item->appendRow(create_item(QString(), false, TreeHandler::displacement_type_tooltip));
    parent->appendRow(item);

    // Create access required item
    item = create_item("Access Required", true, TreeHandler::access_required_tooltip);
    parent->appendRow(item);

    // Create blocked bodyparts item
    item = create_item("Blocked Bodyparts", true, TreeHandler::blocked_bodyparts_tooltip);
    parent->appendRow(item);

    // Create access blocked item
    item = create_item("Access Blocked", true, TreeHandler::access_blocked_tooltip);
    parent->appendRow(item);

    // Create concealed slots item
    item = create_item("Concealed Slots", true, TreeHandler::concealed_slots_tooltip);
    parent->appendRow(item);

    // Add parent to model
    qobject_cast<QStandardItemModel*>(tree->model())->appendRow(parent);
}

void TreeHandler::remove_section(int row)
{
    // Remove an entire BlockedParts section.

    // Easy enough.
    tree->model()->removeRow(row);
    data->removeAt(row);

    // Rename tree items appropriately
    for (int i = 0; i < tree->model()->rowCount(); i++)
        qobject_cast<QStandardItemModel*>(tree->model())->item(i)->setText(QString::number(i));
}

// Tooltip strings
const QString
TreeHandler::access_required_tooltip = "The access required to perform this displacement type.",
TreeHandler::blocked_bodyparts_tooltip = "The body parts that are blocked by this displacement type.",
TreeHandler::access_blocked_tooltip = "The access that this displacement type blocks.",
TreeHandler::concealed_slots_tooltip = "The slots that this displacement type conceals.",
TreeHandler::displacement_type_tooltip = "If this clothing is displaced in the following way, then the 'blocked bodyparts', 'access blocked', and 'concealed preset/slots' will all be revealed.",
TreeHandler::concealed_preset_tooltip = "The preset of slots that this displacement type conceals.",
TreeHandler::widget_tooltip = "This widget allows you to handle the displacement list. There should be at least one 'REMOVE_OR_EQUIP' type section.";
