#ifndef CLOTHINGWINDOW_H
#define CLOTHINGWINDOW_H

#include "clothing/treehandler.h"
#include "clothing/dialoguewidget.h"
#include "common/listviewhandler.h"
#include "common/tagswidget.h"
#include "common/colourswidget.h"
#include "common/effectlisthandler.h"
#include "common/enchantmentwidget.h"

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
    DialogueWidget *dialogue_widget;
    EnchantmentWidget *effect_widget;

    // Widget handlers
    TreeHandler *tree_handler;
    EffectListHandler *effects_list_handler;
    ListViewHandler *incompatible_slot_handler;

    ClothingMod data; // Actual mod data
    QString location; // Active file path

    // Functions
    void save(bool as);
    void set_titles(const QString &title);
    void update_ui();
};

#endif // CLOTHINGWINDOW_H
