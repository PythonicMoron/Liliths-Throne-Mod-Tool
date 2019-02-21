#include "listviewhandler.h"

#include <QMenu>
#include <QStandardItemModel>

ListViewHandler::ListViewHandler(QListView *view, QStringList &list, QAbstractItemDelegate *delegate) : view(view), list(&list)
{
    // Setup view
    view->setItemDelegate(delegate);
    view->setContextMenuPolicy(Qt::CustomContextMenu);

    // Create menu object
    menu = new QMenu();
}
ListViewHandler::~ListViewHandler()
{
    // We don't own the list or the data we have pointers to. Please refrain from destroying what is not yours.

    delete menu;
}

void ListViewHandler::update()
{
    // This function refreshes the list from the data. Useful if data is changed elsewhere. Or to just load the data in the first place.

    // Start with a fresh model.
    if (view->model() != nullptr)
        delete view->model();

    // Setup model
    auto *model = new QStandardItemModel(view);
    QObject::connect(model, &QStandardItemModel::itemChanged, [this] (QStandardItem *item) {modify_data(item->index());});

    // Populate model
    for (const QString &str : *list) {
        model->appendRow(new QStandardItem(str));
    }

    // Add model to view
    view->setModel(model);
}

void ListViewHandler::contex_menu(const QPoint &pos)
{
    // Displays the context menu.

    // Empty the menu.
    menu->clear();

    // Obtain item index using a dark ritual.
    QModelIndex index = view->indexAt(pos);

    // If a valid index is selected, add a remove action
    if (index.isValid()) {
        auto *rem_action = new QAction("Remove entry");
        QObject::connect(rem_action, &QAction::triggered, [this, index] () {remove_item(index);});
        menu->addAction(rem_action);
    }

    // Add an add action
    auto add_action = new QAction("Add entry");
    QObject::connect(add_action, &QAction::triggered, [this] () {add_item();});
    menu->addAction(add_action);

    // Show menu
    menu->popup(view->viewport()->mapToGlobal(pos));
}

void ListViewHandler::modify_data(const QModelIndex &index)
{
    // Modify data

    if (!index.isValid())
        return;
    (*list)[index.row()] = index.data().toString();
}

void ListViewHandler::remove_item(const QModelIndex &index)
{
    // Remove data at index

    if (!index.isValid())
        return;

    view->model()->removeRow(index.row()); // From model
    list->removeAt(index.row()); // From list
}

void ListViewHandler::add_item()
{
    // Add new data

    qobject_cast<QStandardItemModel*>(view->model())->appendRow(new QStandardItem("NULL")); // To model
    list->append(QString()); // To list
}
