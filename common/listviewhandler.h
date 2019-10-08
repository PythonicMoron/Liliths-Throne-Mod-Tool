#ifndef LISTVIEWHANDLER_H
#define LISTVIEWHANDLER_H

#include <QListView>

#include "baseviewhandler.h"

// This is a generic logic class that handles a QStringList via QListView and utilizes custom delegates.

class ListViewHandler : BaseViewHandler<QStringList>
{
public:
    ListViewHandler(QListView *object, QStringList &list, QAbstractItemDelegate *delegate = nullptr);

    // Functions
    void update() override;

protected:
    // Functions
    void modify_data(const QModelIndex &index) override;
    void remove_item(const QModelIndex &index) override;
    void add_item() override;
};

#endif // LISTVIEWHANDLER_H
