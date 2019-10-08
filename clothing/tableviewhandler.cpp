#include "tableviewhandler.h"

#include <QMenu>
#include <QStandardItemModel>

#include "utility.h"

TableViewHandler::TableViewHandler(QTableView *table, QList<QPair<QString, QString>> &table_data, QString header1, QString header2, bool allow_duplicates, QAbstractItemDelegate *delegate1, QAbstractItemDelegate *delegate2) : BaseViewHandler(table, table_data), allow_multiple(allow_duplicates)
{
    // Set headers
    this->header1 = header1;
    this->header2 = header2;

    // Set delegates if optional delegates were given.
    if (delegate1 != nullptr)
        table->setItemDelegateForColumn(0, delegate1);
    if (delegate2 != nullptr)
        table->setItemDelegateForColumn(1, delegate2);
}

void TableViewHandler::update()
{
    // This function loads the current data to the table. Used when the data is changed externally or on initial load.

    // Save headers and start with a fresh model.
    if (view->model() != nullptr)
        delete view->model();

    // Setup model
    auto *model = new QStandardItemModel(view);
    QObject::connect(model, &QStandardItemModel::itemChanged, [this] (QStandardItem *item) {modify_data(item->index());});
    model->setHorizontalHeaderLabels(QStringList({header1, header2}));

    // Populate model
    for (QPair<QString, QString> &item : *data) {
        model->appendRow({new QStandardItem(item.second), new QStandardItem(item.first)});
    }

    // Add model to view
    view->setModel(model);

    // Set column widths
    QTableView* table = qobject_cast<QTableView*>(view);
    table->setColumnWidth(0, 265);
    table->setColumnWidth(1, 265);
}

void TableViewHandler::modify_data(const QModelIndex &index)
{
    // Modify data

    if (index.isValid()) {
        if (index.column() == 0) {
            bool found = false;
            if (!allow_multiple) {
                for (QPair<QString, QString> &pair : *data) {
                    if (pair.second == index.data().toString()) {
                        found = true;
                        view->model()->setData(index, QVariant("NULL"));
                        (*data)[index.row()].second = QString();
                        Utility::error("Can not have duplicate entries!\nPlease change or remove the other item first.");
                        break;
                    }
                }
            }
            if (!found)
                (*data)[index.row()].second = index.data().toString();
        } else
            (*data)[index.row()].first = index.data().toString();
    }
}

void TableViewHandler::remove_item(const QModelIndex &index)
{
    // Remove data at index

    if (index.isValid()) {
        view->model()->removeRow(index.row()); // From model
        data->removeAt(index.row()); // From data
    }
}

void TableViewHandler::add_item()
{
    // Add new data

    qobject_cast<QStandardItemModel*>(view->model())->appendRow({new QStandardItem("NULL"), new QStandardItem("NULL")}); // To model
    data->append(QPair<QString, QString>()); // To data
}
