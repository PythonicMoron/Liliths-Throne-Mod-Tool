#ifndef BASECOMBOBOXDELEGATE_H
#define BASECOMBOBOXDELEGATE_H

#include <QStyledItemDelegate>

// Defines a base combo box delegate. Should not be used directly. Subclass it for use with a Qt View object.

class BaseComboBoxDelegate : public QStyledItemDelegate
{
public:
    BaseComboBoxDelegate(QWidget *parent = nullptr);

    // Function overrides
    virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    virtual void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    virtual void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
    virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif // BASECOMBOBOXDELEGATE_H
