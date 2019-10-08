#ifndef ACCESSIBILITYWIDGET_H
#define ACCESSIBILITYWIDGET_H

#include <QListView>

#include "restrictedsloteditorbase.h"
#include "clothingmod.h"

namespace Ui {
class AccessibilityWidget;
}

class AccessibilityWidget : public RestrictedSlotEditorBase
{
    Q_OBJECT
public:
    explicit AccessibilityWidget(QMap<QString, QList<ClothingMod::BlockedParts>> &data, QWidget *parent);
    ~AccessibilityWidget();

    // Functions
    void open_editor(const QString &slot) override;
    static void setup(const QStringList &displacement_types, const QStringList &access_slots, const QStringList &blocked_bodyparts, const QStringList &concealed_slots, const QStringList &concealed_presets);

public slots:
    // "What's This?" function
    void whats_this();

private:
    enum Context {
        displacement_type_menu, access_required_menu, blocked_bodyparts_menu, blocked_clothing_access_menu, concealed_menu
    };

    Ui::AccessibilityWidget *ui; // UI stuff
    QMap<QString, QList<ClothingMod::BlockedParts>> *map_data; // Pointer to data container
    QList<ClothingMod::BlockedParts> current_item; // Copy of current edit items in data container
    static QStringList displacement_types, access_slots, blocked_bodyparts, concealed_slots, concealed_presets; // These all store a list of flags that are added to the delegates.
    QMenu *menu; // Context menu

    // Tooltip strings
    static const QString concealed_slots_tooltip, concealed_preset_tooltip;

    // More functions
    void save_close();
    void show_section(int index);
    void set_concealed_preset();
    void set_concealed_slots();
    void set_concealed_preset_data(const QModelIndex &index);

    // Context menu
    void context_menu(const QPoint &pos, Context flag);

    // Context menu stuff for the displacement type list
    void type_list_modify_data(const QModelIndex &index);
    void type_list_remove_item(const QModelIndex &index);
    void type_list_add_item();

    // Context menu stuff for the access required, blocked bodyparts, and blocked clothing access lists
    void string_list_modify_data(const QModelIndex &index, QStringList &list);
    void string_list_remove_item(const QModelIndex &index, QListView *view, QStringList &list);
    void string_list_add_item(QListView *view, QStringList &list);

    // Context menu stuff for the concealed slots/preset list
    void concealed_list_modify_data(const QModelIndex &index);
    void concealed_list_remove_item(const QModelIndex &index);
    void concealed_list_add_item(int section);
};

#endif // ACCESSIBILITYWIDGET_H
