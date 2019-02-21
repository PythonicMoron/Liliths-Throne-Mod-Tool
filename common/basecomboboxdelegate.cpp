#include "basecomboboxdelegate.h"

#include <QComboBox>

#include "utility.h"

BaseComboBoxDelegate::BaseComboBoxDelegate(QWidget *parent) : QStyledItemDelegate(parent) {}

QWidget* BaseComboBoxDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& UNUSED(option), const QModelIndex& UNUSED(index)) const
{
    // You MUST override this function.
    return new QComboBox(parent);
}

void BaseComboBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    // This sets the editors selected item or to the first item if data isn't in the editors list.

    auto* cb = qobject_cast<QComboBox*>(editor);
    Q_ASSERT(cb);

    QString currentText = index.data().toString();

    int i = cb->findText(currentText);
    if (i < 0)
        i = 0;

    cb->setCurrentIndex(i);
}

void BaseComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    // This applies the editors text to the model and then it is generally applied to the data.

    auto* cb = qobject_cast<QComboBox*>(editor);
    Q_ASSERT(cb);

    model->setData(index, cb->currentText(), Qt::EditRole);
}

void BaseComboBoxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex& UNUSED(index)) const
{
    // This... updates the editor geomety? I guess...
    editor->setGeometry(option.rect);
}
