#ifndef BASEVIEWHANDLER_H
#define BASEVIEWHANDLER_H

#include <QAbstractItemView>

// Base class for any classes that handle views.
// I got REALLY tired of rewriting this basic layout and the context menu which is exactly the same for most view handlers.

template<class T>
class BaseViewHandler
{
public:
    BaseViewHandler(QAbstractItemView *object, T &object_data);
    virtual ~BaseViewHandler();

    // Functions
    virtual void update();

protected:
    QAbstractItemView *view; // The view we display data on.
    T *data; // The data we are in charge of displaying and editing.
    QMenu *menu; // The context menu object.

    // Functions
    virtual void context_menu(const QPoint &pos);
    virtual void modify_data(const QModelIndex &index);
    virtual void remove_item(const QModelIndex &index);
    virtual void add_item();
};

extern template class BaseViewHandler<QStringList>;
extern template class BaseViewHandler<QList<QPair<QString, QString>>>;

#endif // BASEVIEWHANDLER_H
