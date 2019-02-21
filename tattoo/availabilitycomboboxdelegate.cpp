#include "availabilitycomboboxdelegate.h"

#include <QComboBox>

#include "utility.h"

AvailabilityComboBoxDelegate::AvailabilityComboBoxDelegate(QWidget *parent) : BaseComboBoxDelegate(parent) {}

QWidget *AvailabilityComboBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem& UNUSED(option), const QModelIndex& UNUSED(index)) const
{
    // Simply creates a combo box, fills it with options, then returns it.

    auto *cb = new QComboBox(parent);

    cb->addItems(slot_list);

    return cb;
}

void AvailabilityComboBoxDelegate::setup(const QStringList &slot_list)
{
    // Simple function that sets the data for the combo box. Called from TattooWindow::load_defs
    AvailabilityComboBoxDelegate::slot_list = slot_list;
}

QStringList AvailabilityComboBoxDelegate::slot_list = QStringList();
