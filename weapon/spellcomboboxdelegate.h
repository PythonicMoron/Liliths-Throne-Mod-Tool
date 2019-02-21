#ifndef SPELLCOMBOBOXDELEGATE_H
#define SPELLCOMBOBOXDELEGATE_H

#include "common/basecomboboxdelegate.h"

// Defines a combo box delegate for use with a Qt View object.
// This particular one holds spell flags.

class SpellComboBoxDelegate : public BaseComboBoxDelegate // Most core functionality can be found in BaseComboBoxDelegate!
{
public:
    SpellComboBoxDelegate(QWidget *parent = nullptr);

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

    static void setup(const QStringList &spell_list);

private:
    static QStringList spells; // Stores a list of flags that are added to the delegate on creation.
};

#endif // SPELLCOMBOBOXDELEGATE_H
