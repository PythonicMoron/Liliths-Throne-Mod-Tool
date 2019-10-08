#include "listviewhandler.h"

#include <QMenu>
#include <QStandardItemModel>

ListViewHandler::ListViewHandler(QListView *object, QStringList &list, QAbstractItemDelegate *delegate) : BaseViewHandler(object, list)
{
    // Setup view
    if (delegate != nullptr)
        view->setItemDelegate(delegate);
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
    for (const QString &str : *data) {
        model->appendRow(new QStandardItem(str));
    }

    // Add model to view
    view->setModel(model);
}

void ListViewHandler::modify_data(const QModelIndex &index)
{
    // Modify data

    if (index.isValid())
        (*data)[index.row()] = index.data().toString();
}

void ListViewHandler::remove_item(const QModelIndex &index)
{
    // Remove data at index

    if (index.isValid()) {
        view->model()->removeRow(index.row()); // From model
        data->removeAt(index.row()); // From list
    }
}

void ListViewHandler::add_item()
{
    // Add new data

    qobject_cast<QStandardItemModel*>(view->model())->appendRow(new QStandardItem("NULL")); // To model
    data->append(QString()); // To list
}
