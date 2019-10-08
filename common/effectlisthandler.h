#ifndef EFFECTLISTHANDLER_H
#define EFFECTLISTHANDLER_H

#include <QTableWidget>
#include <QSpinBox>

#include "datacommon.h"
#include "advancedeffectwidget.h"

// This class handles a list of Effect objects via a QListWidget.
// It should be used in conjunction with an EnchantmentWidget object.

class EffectListHandler
{
public:
    EffectListHandler(AdvancedEffectWidget::Mode mode, QTableWidget *widget, QSpinBox *limit_spinbox, QList<DataCommon::Effect> &data);
    ~EffectListHandler();

    // Functions
    void update();
    void reload_editor_ui();

private:
    QTableWidget *widget; // Pointer to widget
    QSpinBox *limit_widget; // Pointer to spin box determining enchantment limit
    QList<DataCommon::Effect> *list; // Pointer to effect list data
    AdvancedEffectWidget *editor; // Editor widget
    QMenu *menu; // Context menu object

    // Functions
    void context_menu(const QPoint &pos, int limit);
    void remove(int row);
    void add();
};

#endif // EFFECTLISTHANDLER_H
