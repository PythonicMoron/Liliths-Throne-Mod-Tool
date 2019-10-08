#ifndef WEAPONWINDOW_H
#define WEAPONWINDOW_H

#include <QWidget>

#include "weapon/weaponmod.h"
#include "common/tagswidget.h"
#include "common/effectlisthandler.h"
#include "common/colourswidget.h"
#include "common/listviewhandler.h"
#include "common/customcomboboxdelegate.h"

// The main class for the weapon widget. Handles the primary functions of the weapon mod window.

namespace Ui {
class WeaponWindow;
}

class WeaponWindow : public QWidget
{
    Q_OBJECT
public:
    explicit WeaponWindow(const QDomDocument &xml_doc, const QString &path, QWidget *parent = nullptr);
    ~WeaponWindow();

    // Functions
    static bool load_defs(bool force_internal = false);
    void populate_ui();

public slots:
    // "What's This?" function
    void whats_this();

private:
    struct UiData {
        UiData();
        QPair<int,int> value_range, damage_range, arcane_cost_range, enchantment_limit_range;
        QStringList rarity_list, clothing_set_list, damage_variance_list, damage_types_list, spells_list;
    };

    Ui::WeaponWindow *ui; // Ui
    static QSharedPointer<UiData> ui_data; // Pointer to UiData.

    // Child widgets
    TagsWidget *tags_widget;
    ColoursWidget *colours_widget;

    // Static delegates
    static QSharedPointer<CustomComboBoxDelegate> damage_delegate, spell_delegate;

    // Widget handlers
    EffectListHandler *effects_list_handler;
    ListViewHandler *damage_type_list_handler, *spell_list_handler, *hit_text_list_handler, *miss_text_list_handler;

    WeaponMod data; // Actual mod data
    QString location; // Active file path

    // Functions
    void save(bool as);
    void update_ui();
};

#endif // WEAPONWINDOW_H
