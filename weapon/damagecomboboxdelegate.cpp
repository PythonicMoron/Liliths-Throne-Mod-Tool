#include "damagecomboboxdelegate.h"

#include <QComboBox>

#include "utility.h"

DamageComboBoxDelegate::DamageComboBoxDelegate(QWidget *parent) : BaseComboBoxDelegate(parent) {}

QWidget* DamageComboBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem& UNUSED(option), const QModelIndex& UNUSED(index)) const
{
    // Simply creates a combo box, fills it with options, then returns it.

    auto *cb = new QComboBox(parent);

    cb->addItems(types);

    return cb;
}

void DamageComboBoxDelegate::setup(const QStringList &damage_types)
{
    // Simple function that sets the data for the combo box. Called from WeaponWindow::load_defs
    DamageComboBoxDelegate::types = damage_types;
}

QStringList DamageComboBoxDelegate::types = QStringList();
