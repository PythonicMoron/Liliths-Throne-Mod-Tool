#ifndef ADVANCEDDIALOGUEWIDGET_H
#define ADVANCEDDIALOGUEWIDGET_H

#include <QPlainTextEdit>

#include "restrictedsloteditorbase.h"
#include "common/listviewhandler.h"
#include "clothingmod.h"

namespace Ui {
class AdvancedDialogueWidget;
}

class AdvancedDialogueWidget : public RestrictedSlotEditorBase
{
    Q_OBJECT
public:
    explicit AdvancedDialogueWidget(QMap<QString, QList<ClothingMod::XPlacementText>> &data, QWidget *parent);
    ~AdvancedDialogueWidget();

    // Functions
    void open_editor(const QString &slot) override;
    static void setup(const QStringList &displacement_types);

public slots:
    // "What's This?" function
    void whats_this();

private:
    Ui::AdvancedDialogueWidget *ui; // Ui
    QMap<QString, QList<ClothingMod::XPlacementText>> *map_data; // Pointer to data container
    QList<ClothingMod::XPlacementText> current_item; // Copy of current edit items in data container
    static QStringList access_types; // Ui data used for populating a combo box
    QMenu *menu; // Context menu

    // More functions
    void save_close();
    void show_section(int index);

    // Context menu specific funtions
    void type_list_context_menu(const QPoint &pos);
    void type_list_modify_data(const QModelIndex &index);
    void type_list_remove_item(const QModelIndex &index);
    void type_list_add_item();
};

#endif // ADVANCEDDIALOGUEWIDGET_H
