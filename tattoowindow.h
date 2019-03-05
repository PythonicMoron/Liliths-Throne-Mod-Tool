#ifndef TATTOOWINDOW_H
#define TATTOOWINDOW_H

#include <QWidget>

#include "tattoo/tattoomod.h"
#include "common/colourswidget.h"
#include "common/listviewhandler.h"

// The main class for the tattoo widget. Handles the primary functions of the tattoo mod window.

namespace Ui {
class TattooWindow;
}

class TattooWindow : public QWidget
{
    Q_OBJECT
public:
    explicit TattooWindow(const QDomDocument &xml_doc, const QString &path, QWidget *parent = nullptr);
    ~TattooWindow();

    // Functions
    static bool load_defs(bool force_internal = false);
    void populate_ui();

public slots:
    // "What's This?" function
    void whats_this();

private:
    struct UiData {
        UiData();
        QPair<int,int> value_range, enchantment_limit_range;
        QStringList inventory_slots;
    };

    Ui::TattooWindow *ui; // Ui
    static QSharedPointer<UiData> ui_data; // Pointer to UiData.

    ColoursWidget *colours_widget; // Child widget
    ListViewHandler *availability_handler; // Widget handler

    TattooMod data; // Actual mod data
    QString location; // Active file path

    // Functions
    void save(bool as);
    void set_titles(const QString &title);
    void update_ui();
};

#endif // TATTOOWINDOW_H
