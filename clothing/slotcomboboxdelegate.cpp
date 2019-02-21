#include "slotcomboboxdelegate.h"

#include <QComboBox>

#include "utility.h"

SlotComboBoxDelegate::SlotComboBoxDelegate(QWidget *parent) : BaseComboBoxDelegate(parent) {}

QWidget* SlotComboBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem& UNUSED(option), const QModelIndex& UNUSED(index)) const
{
    // Simply creates a combo box, fills it with options, then returns it.

    auto *cb = new QComboBox(parent);

    cb->addItems(inventory_slots);

    return cb;
}

void SlotComboBoxDelegate::setup(const QStringList &inventory_slots)
{
    // Simple function that sets the data for the combo box. Called from ClothingWindow::load_defs
    SlotComboBoxDelegate::inventory_slots = inventory_slots;
}

QStringList SlotComboBoxDelegate::inventory_slots = QStringList();
