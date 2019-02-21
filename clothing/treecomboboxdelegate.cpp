#include "treecomboboxdelegate.h"

#include <QComboBox>

#include "utility.h"

TreeComboBoxDelegate::TreeComboBoxDelegate(QWidget *parent) : BaseComboBoxDelegate(parent) {}

QWidget *TreeComboBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem& UNUSED(option), const QModelIndex &index) const
{
    // A bit more complicated than the other combo box delegates.
    // This creates a combo box and fills it with data based on the parent text then returns it.

    auto *cb = new QComboBox(parent);

    QString text = index.parent().data().toString();
    if (text == "Displacement Type")
        cb->addItems(displacement_types);
    else if (text == "Access Required" || text == "Access Blocked")
        cb->addItems(access_slots);
    else if (text == "Blocked Bodyparts")
        cb->addItems(blocked_bodyparts);
    else if (text == "Concealed Slots")
        cb->addItems(concealed_slots);
    else if (text == "Concealed Preset")
        cb->addItems(concealed_presets);

    return cb;
}

void TreeComboBoxDelegate::setup(const QStringList &displacement_types, const QStringList &access_slots, const QStringList &blocked_bodyparts, const QStringList &concealed_slots, const QStringList &concealed_presets)
{
    // Simple function that sets the data for the combo box. Called from ClothingWindow::load_defs
    TreeComboBoxDelegate::displacement_types = displacement_types;
    TreeComboBoxDelegate::access_slots = access_slots;
    TreeComboBoxDelegate::blocked_bodyparts = blocked_bodyparts;
    TreeComboBoxDelegate::concealed_slots = concealed_slots;
    TreeComboBoxDelegate::concealed_presets = concealed_presets;
}

// Static variables. Static variables everywhere.
QStringList TreeComboBoxDelegate::displacement_types = QStringList(),
TreeComboBoxDelegate::access_slots = QStringList(),
TreeComboBoxDelegate::blocked_bodyparts = QStringList(),
TreeComboBoxDelegate::concealed_slots = QStringList(),
TreeComboBoxDelegate::concealed_presets = QStringList();
