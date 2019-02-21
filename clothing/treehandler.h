#ifndef TREEHANDLER_H
#define TREEHANDLER_H

#include <QStandardItemModel>
#include <QTreeView>

#include "clothing/clothingmod.h"

// This class handles a list of ClothingMod::BlockedParts via QTreeView.
// I won't lie, this whole topic has me at a loss. Fuck this class and fuck trees.

class TreeHandler : public QObject
{
    Q_OBJECT
public:
    TreeHandler(QTreeView *view, QList<ClothingMod::BlockedParts> &data);
    ~TreeHandler();

    // Functions
    void update();
    void contex_menu(const QPoint &pos);

private:
    enum Mode {
        access_required, blocked_bodyparts, access_blocked, concealed_slots
        // Used to let the add_entry and remove_entry functions know what the hell is going on.
    };

    // More functions
    QStandardItem* create_item(const QString &title, bool is_parent, const QString &tooltip = nullptr);
    void modify_data(const QModelIndex &index);
    void add_entry(const QModelIndex &index, Mode mode);
    void remove_entry(const QModelIndex &index, Mode mode);
    void set_concealed(const QModelIndex &index, bool preset);
    void add_section();
    void remove_section(int row);

    QTreeView *tree; // Pointer to QTreeView
    QList<ClothingMod::BlockedParts> *data; // Pointer to data in ClothingMod
    QMenu *menu; // Context menu object

    // These are the tooltip strings. Made to be easy and convenient to edit/implement.
    const static QString access_required_tooltip, blocked_bodyparts_tooltip, access_blocked_tooltip, concealed_slots_tooltip, displacement_type_tooltip, concealed_preset_tooltip, header_tooltip;
};

#endif // TREEHANDLER_H
