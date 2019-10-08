#include "customcomboboxdelegate.h"

#include <QComboBox>

#include "utility.h"

CustomComboBoxDelegate::CustomComboBoxDelegate(const QStringList &string_list, QWidget *parent) : QStyledItemDelegate(parent), list(&string_list) {}

CustomComboBoxDelegate::CustomComboBoxDelegate(QWidget *parent) : QStyledItemDelegate(parent) {}

void CustomComboBoxDelegate::setup(const QStringList &string_list)
{
    list = &string_list;
}

QWidget *CustomComboBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem& UNUSED(option), const QModelIndex& UNUSED(index)) const
{
    // Simply creates a combo box, fills it with options, then returns it.

    Q_ASSERT(list);

    auto *cb = new QComboBox(parent);
    cb->addItems(*list);
    return cb;
}

void CustomComboBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    // This sets the editors selected item or to the first item if data isn't in the editors list.

    auto* cb = qobject_cast<QComboBox*>(editor);
    Q_ASSERT(cb);

    int i = cb->findText(index.data().toString());
    if (i < 0)
        i = 0;

    cb->setCurrentIndex(i);
}

void CustomComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    // This applies the editors text to the model and then it is generally applied to the data.

    auto* cb = qobject_cast<QComboBox*>(editor);
    Q_ASSERT(cb);

    model->setData(index, cb->currentText(), Qt::EditRole);
}

void CustomComboBoxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex& UNUSED(index)) const
{
    // This... updates the editor geomety? I guess...
    editor->setGeometry(option.rect);
}
