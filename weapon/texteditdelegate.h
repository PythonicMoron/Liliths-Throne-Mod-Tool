#ifndef TEXTEDITDELEGATE_H
#define TEXTEDITDELEGATE_H

#include <QStyledItemDelegate>

// Defines a plain text edit delegate for use with a Qt View object.

class TextEditDelegate : public QStyledItemDelegate
{
public:
    TextEditDelegate(QWidget *parent = nullptr);

    // Function overrides
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif // TEXTEDITDELEGATE_H
