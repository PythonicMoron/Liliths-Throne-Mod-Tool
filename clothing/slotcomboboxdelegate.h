#ifndef SLOTCOMBOBOXDELEGATE_H
#define SLOTCOMBOBOXDELEGATE_H

#include "common/basecomboboxdelegate.h"

// Defines a combo box delegate for use with a Qt View object.
// This particular one holds inventory slot flags.

class SlotComboBoxDelegate : public BaseComboBoxDelegate // Most core functionality can be found in BaseComboBoxDelegate!
{
public:
    SlotComboBoxDelegate(QWidget *parent = nullptr);

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

    static void setup(const QStringList &inventory_slots);

private:
    static QStringList inventory_slots; // Stores a list of flags that are added to the delegate on creation.
};

#endif // SLOTCOMBOBOXDELEGATE_H
