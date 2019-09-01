#ifndef COLOURSWIDGET_H
#define COLOURSWIDGET_H

#include <QWidget>

#include "common/colour.h"

// This class holds the logic for the colours widget.
// This might be replaced with something else. Not sure I like it.

namespace Ui {
class ColoursWidget;
}

class ColoursWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ColoursWidget(QWidget *parent);
    ~ColoursWidget();

    // Functions
    void open(Colour *target, bool is_preset);
    void reload_ui();
    static bool load_colours(bool force_internal = false);

    // Some other classes need the raw colour and preset data, so we must expose it.
    static QStringList possible_colours();
    static QStringList possible_presets();

public slots:
    // "What's This?" function
    void whats_this();

protected:
    // Override closeEvent to intercept things like pressing the 'x' button.
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::ColoursWidget *ui; // Ui
    bool showing_colours; // State of the ui
    Colour *colour_ptr; // Pointer to colour object in question

    static QStringList colours, presets; // Ui data used for populating the list widget.

    // Functions
    void show_presets();
    void show_colours();
    void apply();
};

#endif // COLOURSWIDGET_H
