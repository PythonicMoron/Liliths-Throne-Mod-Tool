#include "texteditdelegate.h"

#include <QPlainTextEdit>

#include "utility.h"

TextEditDelegate::TextEditDelegate(QWidget *parent) : QStyledItemDelegate(parent) {}

QWidget* TextEditDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& UNUSED(option), const QModelIndex& UNUSED(index)) const
{
    // Create the QPlainTextEdit widget and return it.
    return new QPlainTextEdit(parent);
}

void TextEditDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    // This sets the editors text.

    auto *pte = qobject_cast<QPlainTextEdit*>(editor);
    Q_ASSERT(pte);

    pte->setPlainText(index.data().toString());
}

void TextEditDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    // This applies the editors text to the model and then it is generally applied to the data.

    auto *pte = qobject_cast<QPlainTextEdit*>(editor);
    Q_ASSERT(pte);

    model->setData(index, pte->toPlainText(), Qt::EditRole);
}

void TextEditDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex& UNUSED(index)) const
{
    // This... updates the editor geomety? I guess...
    editor->setGeometry(option.rect);
}
