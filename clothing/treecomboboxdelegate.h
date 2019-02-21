#ifndef COMBOBOXDELEGATE_H
#define COMBOBOXDELEGATE_H

#include "common/basecomboboxdelegate.h"

// Defines a combo box delegate for use with a Qt View object.
// This particular one holds flags for the various fields in ClothingMod::BlockedParts.

class TreeComboBoxDelegate : public BaseComboBoxDelegate // Most core functionality can be found in BaseComboBoxDelegate!
{
public:
    explicit TreeComboBoxDelegate(QWidget *parent = nullptr);

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

    static void setup(const QStringList &displacement_types, const QStringList &access_slots, const QStringList &blocked_bodyparts, const QStringList &concealed_slots, const QStringList &concealed_presets);

private:
    static QStringList displacement_types, access_slots, blocked_bodyparts, concealed_slots, concealed_presets; // These all store a list of flags that are added to the delegate on creation.
};

#endif // COMBOBOXDELEGATE_H
