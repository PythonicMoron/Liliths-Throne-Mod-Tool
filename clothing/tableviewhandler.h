#ifndef TABLEHANDLER_H
#define TABLEHANDLER_H

#include <QTableView>

#include "common/baseviewhandler.h"

// Generic handler for the table widgets in the clothing window. Designed for 2 by X tables. Uses custom delegates in each column.
// Nearly a direct copy of the ListViewHandler class. Minor changes in how the underlying data is handled, otherwise it is identical.

class TableViewHandler : BaseViewHandler<QList<QPair<QString, QString>>>
{
public:
    TableViewHandler(QTableView *table, QList<QPair<QString, QString>> &data, QString header1, QString header2, bool allow_duplicates, QAbstractItemDelegate *delegate1 = nullptr, QAbstractItemDelegate *delegate2 = nullptr);

    // Functions
    void update() override;

private:
    // Headers
    QString header1, header2;

    // Vars
    bool allow_multiple;

    // Functions
    void modify_data(const QModelIndex &index) override;
    void remove_item(const QModelIndex &index) override;
    void add_item() override;
};

#endif // TABLEHANDLER_H
