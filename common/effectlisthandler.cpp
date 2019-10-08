#include "effectlisthandler.h"

#include <QMenu>

EffectListHandler::EffectListHandler(AdvancedEffectWidget::Mode mode, QTableWidget *widget, QSpinBox *limit_spinbox, QList<DataCommon::Effect> &data) : widget(widget), limit_widget(limit_spinbox), list(&data)
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

    // Create the editor and connect the edit trgger
    editor = new AdvancedEffectWidget(mode, &data, widget->window());
    QObject::connect(widget, &QTableWidget::itemDoubleClicked, [this] (QTableWidgetItem *item) {editor->open(item->row());});

    // Construct a menu object.
    menu = new QMenu();

    // Connections
    QObject::connect(widget, &QTableWidget::customContextMenuRequested, [this] (const QPoint &pos) {context_menu(pos, limit_widget->value());});
    QObject::connect(editor, &AdvancedEffectWidget::data_changed, [this] () {update();});
}

EffectListHandler::~EffectListHandler()
{
    // We don't own the table widget, so let's NOT call delete on it.

    delete menu;
}

void EffectListHandler::update()
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
    widget->setRowCount(list->count());

    // Apply previous column sizes
    for (int i = 0; i < sizes.count(); i++)
        widget->setColumnWidth(i, sizes[i]);

    // Populate
    for (int i = 0; i < list->count(); i++) {
        widget->setItem(i, 0, new QTableWidgetItem(list->at(i).primary_modifier));
        widget->setItem(i, 1, new QTableWidgetItem(list->at(i).secondary_modifier));
        widget->setItem(i, 2, new QTableWidgetItem(list->at(i).potency));
        widget->setItem(i, 3, new QTableWidgetItem(QString::number(list->at(i).limit)));
        widget->setItem(i, 4, new QTableWidgetItem(QString::number(list->at(i).timer)));
    }
}

void EffectListHandler::reload_editor_ui()
{
    // Proxy function for editor widget
    editor->reload_ui();
}

void EffectListHandler::context_menu(const QPoint &pos, int limit)
{
    // Displays a context menu.

    // Empty the menu
    menu->clear();

    // If the index points to a valid item, add the option to remove it.
    int index = widget->indexAt(pos).row();
    if (index >= 0 && index < widget->rowCount()) {
        // Create and add 'remove' function
        QAction *remove_action = new QAction("Remove selected");
        QObject::connect(remove_action, &QAction::triggered, [this, index] () {remove(index);});
        menu->addAction(remove_action);
    }

    // Add the option to add a new item if the effect count is within our limit or the limit is disabled.
    if (limit < 0 || (widget->rowCount() < limit && widget->rowCount() <= 100)) {
        // Create and add 'add' function
        QAction *add_action = new QAction("Add new");
        QObject::connect(add_action, &QAction::triggered, [this] () {add();});
        menu->addAction(add_action);
    }

    // Display menu
    menu->popup(widget->viewport()->mapToGlobal(pos));
}

void EffectListHandler::remove(int row)
{
    // Remove widget row and data at index

    widget->removeRow(row);
    list->removeAt(row);
}

void EffectListHandler::add()
{
    // Add row to widget and entry in data list

    // Data
    list->append(DataCommon::Effect());

    // Widget
    int index = widget->rowCount();
    widget->insertRow(index);
    widget->setItem(index, 0, new QTableWidgetItem(list->at(index).primary_modifier));
    widget->setItem(index, 1, new QTableWidgetItem(list->at(index).secondary_modifier));
    widget->setItem(index, 2, new QTableWidgetItem(list->at(index).potency));
    widget->setItem(index, 3, new QTableWidgetItem(QString::number(list->at(index).limit)));
    widget->setItem(index, 4, new QTableWidgetItem(QString::number(list->at(index).timer)));
}
