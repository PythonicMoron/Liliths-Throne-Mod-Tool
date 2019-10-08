#ifndef CUSTOMCOMBOBOXDELEGATE_H
#define CUSTOMCOMBOBOXDELEGATE_H

#include <QStyledItemDelegate>

class CustomComboBoxDelegate : public QStyledItemDelegate
{
public:
    CustomComboBoxDelegate(const QStringList &string_list, QWidget *parent = nullptr);
    CustomComboBoxDelegate(QWidget *parent = nullptr);

    // Functions
    void setup(const QStringList &string_list);

    // Function overrides
    virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    virtual void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    virtual void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
    virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    const QStringList *list;
};

#endif // CUSTOMCOMBOBOXDELEGATE_H
