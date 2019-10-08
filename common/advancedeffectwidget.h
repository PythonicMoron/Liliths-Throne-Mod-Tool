#ifndef ADVANCEDEFFECTWIDGET_H
#define ADVANCEDEFFECTWIDGET_H

#include <QWidget>

#include "datacommon.h"

namespace Ui {
class AdvancedEffectWidget;
}

class AdvancedEffectWidget : public QWidget
{
    Q_OBJECT

    // The Effect class stores data for an effect. Shocking, I know.
    // Not to be confused with DataCommon::Effect. This Effect class relates to effects defined in the game that you can store in a DataCommon::Effect object.
    // It also implements a way to have a large number of Effect objects without having an overwhelming amount of QStrings in memory.
    class Effect {
    public:
        Effect();
        Effect(int type, QList<int> &secondary, QList<int> &potency, QPair<int, int> &limit, QPair<int, int> &timer);
        Effect(const Effect &other);

        // Lots of functions.
        static int potency_index(const QString &str);
        static int secondary_index(const QString &str);
        static int effect_index(const QString &str);

        // Lots.
        QStringList get_secondary_mods();
        QList<int> get_secondary_mods_index();

        // Even more.
        QString get_effect_type();
        int get_effect_type_index();

        // These are all seperated for a reason. Mostly readability, but a reason nonetheless.
        QStringList get_potency_options();
        QList<int> get_potency_options_index();

        // The range limits are public because I'm lazy and look at that mess up there. Fuck that.
        // They also don't need to be guarded like the effectively unusable index data below.
        QPair<int,int> limit_range, timer_range;

    private:
        // These contain the actual strings/flags referred to by the index data below.
        // There probably already exists a similar/better system to do this, but I don't know it, so I'm using mine.
        static QStringList potency_strings, secondary_strings, type_strings;

        // These are actually index numbers for the static QStringLists above. This was done so I don't have 40 different strings all with the same exact content.
        // As a quick example, if effect_type is '5' then the string you are looking for would be in type_strings[5].
        int effect_type;
        QList<int> potency_options, secondary_mods;
    };

public:
    enum Mode { // This is used for array indexing! Take caution. A lot of it.
        clothing, weapon,
        LAST // I am used for size comparison. Don't move me.
    };

    explicit AdvancedEffectWidget(Mode mode, QList<DataCommon::Effect> *data, QWidget *parent);
    ~AdvancedEffectWidget();

    // Functions
    void open(int index);
    void reload_ui();
    static bool load_effects(bool force_internal = false);

public slots:
    // "What's This?" function
    void whats_this();

signals:
    void data_changed(); // Emitted when the editor is closed and the data is saved.

protected:
    // Override closeEvent to intercept things like pressing the 'x' button.
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::AdvancedEffectWidget *ui; // UI
    Mode mode; // This determines which set of effects this object will use.
    QList<DataCommon::Effect> *data_list; // Data list
    QSharedPointer<DataCommon::Effect> current_item; // Temp item
    int data_index; // Index we are editing

    // A nightmare. This associates all of the effects with their primary modifier. I need to find a better way to do this.
    static QList<QMultiMap<QString, QSharedPointer<Effect>>> effect_maps;

    // Functions again.
    void populate_secondary();
    void populate_ui();
    void save_close();
};

#endif // ADVANCEDEFFECTWIDGET_H
