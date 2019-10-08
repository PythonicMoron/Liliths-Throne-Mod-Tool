#include "restrictedslotlistviewhandler.h"

#include <QMenu>
#include <QStandardItemModel>

#include "utility.h"

template<class T>
RestrictedSlotListViewHandler<T>::RestrictedSlotListViewHandler(QListView *list, QMap<QString, QList<T>> &data, QStringList &slot_list, RestrictedSlotEditorBase *editor) : view(list), data_map(&data), available_slots(&slot_list), edit_widget(editor)
{
    // Setup view
    view->setContextMenuPolicy(Qt::CustomContextMenu);

    // Create menu object
    menu = new QMenu();

    // Connections
    QObject::connect(list, &QListView::doubleClicked, [this] (const QModelIndex &index) {edit_widget->open_editor(index.data().toString()); view->window()->hide();});
    QObject::connect(list, &QListView::customContextMenuRequested, [this] (const QPoint &pos) {context_menu(pos);});
}

template<class T>
RestrictedSlotListViewHandler<T>::~RestrictedSlotListViewHandler()
{
    // The list, map data, and string list data pointers are handled by other more qualified sources. Don't touch them here.

    delete menu;
}

template<class T>
void RestrictedSlotListViewHandler<T>::update()
{
    // This function updates the list from the data. Called when the data is changed elsewhere or on initial load.

    // Start with a fresh model.
    if (!view->model())
        delete view->model();

    // Setup model
    auto *model = new QStandardItemModel(view);

    // Populate model
    for (const QString &key : data_map->keys()) {
        model->appendRow(new QStandardItem(key));
    }

    // Add model to view
    view->setModel(model);
}

template<class T>
void RestrictedSlotListViewHandler<T>::context_menu(const QPoint &pos)
{
    // Displays the context menu.

    // Empty the menu.
    menu->clear();

    // Obtain item index using a dark ritual.
    QModelIndex index = view->indexAt(pos);

    // If a valid index is selected, add 'remove', 'edit', and 'change slot' actions
    if (index.isValid()) {
        // Remove action
        QAction *rem_action = menu->addAction("Remove entry");
        QObject::connect(rem_action, &QAction::triggered, [this, index] () {remove_item(index);});

        // Edit action
        QAction *edit_action = menu->addAction("Edit");
        QObject::connect(edit_action, &QAction::triggered, [this, index] () {edit_widget->open_editor(index.data().toString()); view->window()->hide();});

        // Change slot action
        QMenu *change_menu = menu->addMenu("Change slot...");
        for (const QString &slot : *available_slots) {
            QAction *change_action = change_menu->addAction(slot);
            QObject::connect(change_action, &QAction::triggered, [this, change_action, index] () {change_slot(index, change_action->text());});
        }
    }

    // Add submenu and populate it with options. Also ensures that no duplicate slot options show up and does not show if no options are available
    QMenu *add_menu = new QMenu("Add entry...", menu);
    for (const QString &slot : *available_slots) {
        if (view->model()->match(view->model()->index(0, 0), Qt::DisplayRole, QVariant(slot), 1, Qt::MatchWrap | Qt::MatchFixedString).isEmpty()) {
            QAction *slot_action = add_menu->addAction(slot);
            QObject::connect(slot_action, &QAction::triggered, [this, slot_action] () {add_item(slot_action->text());});
        }
    }
    if (!add_menu->isEmpty())
        menu->addMenu(add_menu);

    // Show menu
    menu->popup(view->viewport()->mapToGlobal(pos));
}

template<class T>
void RestrictedSlotListViewHandler<T>::remove_item(const QModelIndex &index)
{
    // Remove data at index

    if (index.isValid()) {
        data_map->remove(index.data().toString()); // From data map
        view->model()->removeRow(index.row()); // From model
    }
}

template<class T>
void RestrictedSlotListViewHandler<T>::add_item(const QString &slot)
{
    // Simply construct a new item and add it to the model and data map

    data_map->insert(slot, QList<T>()); // To data map
    qobject_cast<QStandardItemModel*>(view->model())->appendRow(new QStandardItem(slot)); // To model
}

template<class T>
void RestrictedSlotListViewHandler<T>::change_slot(const QModelIndex &index, const QString &slot)
{
    // Change an entrys associated slot

    // We have to make sure the slot isn't already used
    if (data_map->contains(slot)) {
        Utility::error("You can not switch to a slot that is in use already! Please delete or change that slot first.");
        return;
    }

    // First, copy data and remove old entry
    QList<T> data = data_map->take(index.data().toString());

    // Second, add under new key
    data_map->insert(slot, data);

    // Finally, update model
    qobject_cast<QStandardItemModel*>(view->model())->itemFromIndex(index)->setData(slot);
}

template class RestrictedSlotListViewHandler<ClothingMod::BlockedParts>;
template class RestrictedSlotListViewHandler<ClothingMod::XPlacementText>;
