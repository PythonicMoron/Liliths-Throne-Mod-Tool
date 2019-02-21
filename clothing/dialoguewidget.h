#ifndef DIALOGUEWIDGET_H
#define DIALOGUEWIDGET_H

#include <QWidget>

#include "clothing/clothingmod.h"

// This class holds the logic for the dialogue widget.
// I think this all needs to be redone as the ui is very unintuitive and the class is a mess.

namespace Ui {
class DialogueWidget;
}

class DialogueWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DialogueWidget(QWidget *parent);
    ~DialogueWidget();

    // Functions
    static void setup(const QStringList &displacement_types);
    void open(QList<ClothingMod::XPlacementText> &dialogue);
    void reload_ui();

protected:
    // Override closeEvent to intercept things like pressing the 'x' button.
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::DialogueWidget *ui; // Ui
    QList<ClothingMod::XPlacementText> *dialogue; // Pointer to list of XPlacementText. Should point to said object inside ClothingMod.
    static QStringList access_types; // Ui data used for populating a combo box.

    // Functions
    void reset_ui();
    void open_section(int index);
    void remove_current();
    void add_new();
};

#endif // DIALOGUEWIDGET_H
