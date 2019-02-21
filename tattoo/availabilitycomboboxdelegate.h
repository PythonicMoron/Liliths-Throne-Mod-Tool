#ifndef AVAILABILITYCOMBOBOXDELEGATE_H
#define AVAILABILITYCOMBOBOXDELEGATE_H

#include "common/basecomboboxdelegate.h"

// Defines a combo box delegate for use with a Qt View object.
// This particular one holds inventory slot flags.

// Note: This is NOT identical to 'clothing/slotcomboboxdelegate'! They are similar, but not exactly the same.

class AvailabilityComboBoxDelegate : public BaseComboBoxDelegate // Most core functionality can be found in BaseComboBoxDelegate!
{
public:
    AvailabilityComboBoxDelegate(QWidget *parent = nullptr);
    
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    
    static void setup(const QStringList &slot_list);
    
private:
    static QStringList slot_list; // Stores a list of flags that are added to the delegate on creation.
};

#endif // AVAILABILITYCOMBOBOXDELEGATE_H
