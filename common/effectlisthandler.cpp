#include "effectlisthandler.h"

#include <QMenu>

EffectListHandler::EffectListHandler(QListWidget *widget) : QObject(widget), widget(widget)
{
    // Setup
    widget->setContextMenuPolicy(Qt::CustomContextMenu);

    // Construct a menu object.
    menu = new QMenu();
}

EffectListHandler::~EffectListHandler()
{
    // We don't own the list widget, so let's NOT call delete on it.

    delete menu;
}

void EffectListHandler::update(const QList<DataCommon::Effect> &effects)
{
    // This empties the list and repopulates it from the given list of effects.

    widget->clear();
    for (const DataCommon::Effect &effect : effects) {
        QString line = QString();
        line.append("Primary mod: " + effect.primary_modifier + ", Secondary mod: " + effect.secondary_modifier + ", Potency: " + effect.potency);
        if (effect.limit != 0)
            line.append(", Limit: " + QString::number(effect.limit));
        if (effect.timer != 0)
            line.append(", Timer: " + QString::number(effect.timer));
        if (line.isEmpty())
            line = "NULL";
        widget->addItem(line);
    }
}

void EffectListHandler::contex_menu(const QPoint &pos, int limit)
{
    // Displays a context menu.

    // Empty the menu
    menu->clear();

    // If the index points to a valid item, add the option to remove it.
    int index = widget->indexAt(pos).row();
    if (index >= 0 && index < widget->count()) {
        // Create and add 'remove' function
        QAction *remove_action = new QAction("Remove selected");
        connect(remove_action, &QAction::triggered, [this, index] () {emit remove(index);});
        menu->addAction(remove_action);
    }

    // Add the option to add a new item.
    if (widget->count() < limit) {
        // Create and add 'add' function
        QAction *add_action = new QAction("Add new");
        connect(add_action, &QAction::triggered, [this] () {emit add();});
        menu->addAction(add_action);
    }

    // Display menu
    menu->popup(widget->viewport()->mapToGlobal(pos));
}
