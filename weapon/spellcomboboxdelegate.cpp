#include "spellcomboboxdelegate.h"

#include <QComboBox>

#include "utility.h"

SpellComboBoxDelegate::SpellComboBoxDelegate(QWidget *parent) : BaseComboBoxDelegate(parent) {}

QWidget *SpellComboBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem& UNUSED(option), const QModelIndex& UNUSED(index)) const
{
    // Simply creates a combo box, fills it with options, then returns it.

    auto *cb = new QComboBox(parent);

    cb->addItems(spells);

    return cb;
}

void SpellComboBoxDelegate::setup(const QStringList &spell_list)
{
    // Simple function that sets the data for the combo box. Called from WeaponWindow::load_defs
    spells = spell_list;
}

QStringList SpellComboBoxDelegate::spells = QStringList();
