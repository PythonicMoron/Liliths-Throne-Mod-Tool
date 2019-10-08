#include "baseviewhandler.h"

#include <QMenu>

#include "utility.h"
#include "notimplementedexception.h"

template<class T>
BaseViewHandler<T>::BaseViewHandler(QAbstractItemView *object, T &object_data) : view(object), data(&object_data)
{
    // Setup view
    view->setContextMenuPolicy(Qt::CustomContextMenu);

    // Create menu object
    menu = new QMenu(object);

    // Connections
    QObject::connect(view, &QAbstractItemView::customContextMenuRequested, [this] (const QPoint &pos) {context_menu(pos);});
}

template<class T>
BaseViewHandler<T>::~BaseViewHandler() {}

template<class T>
void BaseViewHandler<T>::update()
{
    // Implement me!
    throw NotImplementedException();
}

template<class T>
void BaseViewHandler<T>::context_menu(const QPoint &pos)
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

template<class T>
void BaseViewHandler<T>::modify_data(const QModelIndex& UNUSED(index))
{
    // Implement me!
    throw NotImplementedException();
}

template<class T>
void BaseViewHandler<T>::remove_item(const QModelIndex& UNUSED(index))
{
    // Implement me!
    throw NotImplementedException();
}

template<class T>
void BaseViewHandler<T>::add_item()
{
    // Implement me!
    throw NotImplementedException();
}

template class BaseViewHandler<QStringList>;
template class BaseViewHandler<QList<QPair<QString, QString>>>;
