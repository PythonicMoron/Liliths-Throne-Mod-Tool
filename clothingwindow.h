#ifndef CLOTHINGWINDOW_H
#define CLOTHINGWINDOW_H

#include "clothing/tableviewhandler.h"
#include "clothing/restrictedslotlistviewhandler.h"
#include "clothing/accessibilitywidget.h"
#include "clothing/advanceddialoguewidget.h"
#include "common/listviewhandler.h"
#include "common/tagswidget.h"
#include "common/colourswidget.h"
#include "common/effectlisthandler.h"
#include "common/customcomboboxdelegate.h"

// The main class for the clothing widget. Handles the primary functions of the clothing mod window.

namespace Ui {
class ClothingWindow;
}

class ClothingWindow : public QWidget
{
    Q_OBJECT
public:
    explicit ClothingWindow(const QDomDocument &xml_doc, const QString &path, QWidget *parent = nullptr);
    ~ClothingWindow();

    // Functions
    static bool load_defs(bool force_internal = false);
    void populate_ui();

public slots:
    // "What's This?" function
    void whats_this();

private:
    // This struct contains the data for the ui.
    struct UiData {
        UiData();
        QPair<int, int> value_range, physical_resistance_range, enchantment_limit_range;
        QStringList slot_list, gender_list, rarity_list, clothing_set_list, displacement_types, access_slots, blocked_bodyparts, concealed_presets;
    };

    Ui::ClothingWindow *ui; // Ui
    static QSharedPointer<UiData> ui_data; // Pointer to UiData.

    // Child widgets
    ColoursWidget *colours_widget;
    TagsWidget *tags_widget;
    AccessibilityWidget *access_widget;
    AdvancedDialogueWidget *dialogue_widget;

    // Static delegates
    static QSharedPointer<CustomComboBoxDelegate> slot_delegate, tags_delegate;

    // Widget handlers
    EffectListHandler *effects_list_handler;
    ListViewHandler *equippable_slot_handler, *default_pattern_handler;
    TableViewHandler *incompatible_slot_handler, *slot_tag_handler, *equip_image_handler;
    RestrictedSlotListViewHandler<ClothingMod::BlockedParts> *access_slot_handler;
    RestrictedSlotListViewHandler<ClothingMod::XPlacementText> *dialogue_slot_handler;

    ClothingMod data; // Actual mod data
    QString location; // Active file path

    // Functions
    void save(bool as);
    void update_ui();
};

#endif // CLOTHINGWINDOW_H
