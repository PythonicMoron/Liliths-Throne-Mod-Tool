#ifndef ACCESSSLOTLISTVIEWHANDLER_H
#define ACCESSSLOTLISTVIEWHANDLER_H

#include <QListView>

#include "clothingmod.h"
#include "restrictedsloteditorbase.h"

template<class T>
class RestrictedSlotListViewHandler
{
public:
    RestrictedSlotListViewHandler(QListView *list, QMap<QString, QList<T>> &data, QStringList &slot_list, RestrictedSlotEditorBase *editor);
    ~RestrictedSlotListViewHandler();

    // Functions
    void update();
    void context_menu(const QPoint &pos);

private:
    QListView *view;
    QMap<QString, QList<T>> *data_map;
    QStringList *available_slots;
    RestrictedSlotEditorBase *edit_widget;
    QMenu *menu;

    // Functions
    void modify_data(const QModelIndex &index);
    void remove_item(const QModelIndex &index);
    void add_item(const QString &slot);
    void change_slot(const QModelIndex &index, const QString &slot);
};

#endif // ACCESSSLOTLISTVIEWHANDLER_H
