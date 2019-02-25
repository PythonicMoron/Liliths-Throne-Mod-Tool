#include "effectlisthandler.h"

#include <QMenu>

EffectListHandler::EffectListHandler(QTableWidget *widget) : QObject(widget), widget(widget)
{
    // Setup
    widget->setContextMenuPolicy(Qt::CustomContextMenu);
    widget->setColumnCount(5); // Primary mod, secondary mod, potency, limit, timer
    widget->setHorizontalHeaderLabels(QStringList({"Primary Modifier", "Secondary Modifier", "Potency", "Limit", "Timer"}));

    // Default sizes
    widget->setColumnWidth(0, 175);
    widget->setColumnWidth(1, 175);
    widget->setColumnWidth(2, 90);
    widget->setColumnWidth(3, 25);
    widget->setColumnWidth(4, 25);

    // Construct a menu object.
    menu = new QMenu();
}

EffectListHandler::~EffectListHandler()
{
    // We don't own the table widget, so let's NOT call delete on it.

    delete menu;
}

void EffectListHandler::update(const QList<DataCommon::Effect> &effects)
{
    // This empties the table and repopulates it from the given list of effects.

    // Save current column sizes as the clear method removes this attribute
    auto sizes = QVector<int>();
    for (int i = 0; i < widget->columnCount(); i++)
        sizes.append(widget->columnWidth(i));

    // Empty table then reset column data and headers
    widget->clear();
    widget->setColumnCount(5); // Primary mod, secondary mod, potency, limit, timer
    widget->setHorizontalHeaderLabels(QStringList({"Primary Modifier", "Secondary Modifier", "Potency", "Limit", "Timer"}));
    widget->setRowCount(effects.count());

    // Apply previous column sizes
    for (int i = 0; i < sizes.count(); i++)
        widget->setColumnWidth(i, sizes[i]);

    // Populate
    for (int i = 0; i < effects.count(); i++) {
        widget->setItem(i, 0, new QTableWidgetItem(effects[i].primary_modifier));
        widget->setItem(i, 1, new QTableWidgetItem(effects[i].secondary_modifier));
        widget->setItem(i, 2, new QTableWidgetItem(effects[i].potency));
        widget->setItem(i, 3, new QTableWidgetItem(QString::number(effects[i].limit)));
        widget->setItem(i, 4, new QTableWidgetItem(QString::number(effects[i].timer)));
    }
}

void EffectListHandler::contex_menu(const QPoint &pos, int limit)
{
    // Displays a context menu.

    // Empty the menu
    menu->clear();

    // If the index points to a valid item, add the option to remove it.
    int index = widget->indexAt(pos).row();
    if (index >= 0 && index < widget->rowCount()) {
        // Create and add 'remove' function
        QAction *remove_action = new QAction("Remove selected");
        connect(remove_action, &QAction::triggered, [this, index] () {emit remove(index);});
        menu->addAction(remove_action);
    }

    // Add the option to add a new item.
    if (widget->rowCount() < limit) {
        // Create and add 'add' function
        QAction *add_action = new QAction("Add new");
        connect(add_action, &QAction::triggered, [this] () {emit add();});
        menu->addAction(add_action);
    }

    // Display menu
    menu->popup(widget->viewport()->mapToGlobal(pos));
}
