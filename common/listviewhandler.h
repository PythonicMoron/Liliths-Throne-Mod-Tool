#ifndef LISTVIEWHANDLER_H
#define LISTVIEWHANDLER_H

#include <QListView>

// This is a generic logic class that handles a QStringList via QListView and utilizes custom delegates.

class ListViewHandler
{
public:
    ListViewHandler(QListView *view, QStringList &list, QAbstractItemDelegate *delegate);
    virtual ~ListViewHandler();

    // Functions
    virtual void update();
    virtual void contex_menu(const QPoint &pos);

protected:
    QListView *view; // The QListView we display data on.
    QStringList *list; // The data we are in charge of displaying and editing.
    QMenu *menu; // The context menu object.

    // Functions
    void modify_data(const QModelIndex &index);
    void remove_item(const QModelIndex &index);
    void add_item();
};

#endif // LISTVIEWHANDLER_H
