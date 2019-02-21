#ifndef EFFECTLISTHANDLER_H
#define EFFECTLISTHANDLER_H

#include <QListWidget>

#include "common/datacommon.h"

// This class handles a list of Effect objects via a QListWidget.
// It should be used in conjunction with an EnchantmentWidget object.

class EffectListHandler : public QObject
{
    Q_OBJECT
public:
    EffectListHandler(QListWidget *widget);
    ~EffectListHandler();

    // Functions
    void update(const QList<DataCommon::Effect> &effects);
    void contex_menu(const QPoint &pos, int limit);

signals:
    // You need to tie these signals into the mod data and an EnchantmentWidget with connections like these:
    // connect(effects_list_handler, &EffectListHandler::remove, [this] (int index) {data.effects.removeAt(index); effects_list_handler->update(data.effects);});
    // connect(effects_list_handler, &EffectListHandler::add, [this] () {data.effects.append(DataCommon::Effect()); effect_widget->open(&data.effects.last());});

    void remove(int i);
    void add();

private:
    QListWidget *widget; // Pointer to QListWidget
    QMenu *menu; // Context menu object
};

#endif // EFFECTLISTHANDLER_H
