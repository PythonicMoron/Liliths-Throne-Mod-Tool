#ifndef DAMAGECOMBOBOXDELEGATE_H
#define DAMAGECOMBOBOXDELEGATE_H

#include "common/basecomboboxdelegate.h"

// Defines a combo box delegate for use with a Qt View object.
// This particular one holds damage type flags.

class DamageComboBoxDelegate : public BaseComboBoxDelegate // Most core functionality can be found in BaseComboBoxDelegate!
{
public:
    DamageComboBoxDelegate(QWidget *parent = nullptr);

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

    static void setup(const QStringList &damage_types);

private:
    static QStringList types; // Stores a list of flags that are added to the delegate on creation.
};

#endif // DAMAGECOMBOBOXDELEGATE_H
