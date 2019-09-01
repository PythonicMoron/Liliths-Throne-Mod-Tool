#ifndef OUTFITWINDOW_H
#define OUTFITWINDOW_H

#include <QWidget>
#include <QDomDocument>

#include "common/colourswidget.h"
#include "outfit/outfitmod.h"

namespace Ui {
class OutfitWindow;
}

class OutfitWindow : public QWidget
{
    Q_OBJECT

public:
    explicit OutfitWindow(const QDomDocument &xml_doc, const QString &path, QWidget *parent = nullptr);
    ~OutfitWindow();

    // Functions
    static bool load_defs(bool force_internal = false);
    void populate_ui();

public slots:
    // "What's This?" function
    void whats_this();

private:
    struct UiData {
        //
    };

    Ui::OutfitWindow *ui; // Ui
    static QSharedPointer<UiData> ui_data; // Pointer to UiData.

    // Child widgets
    ColoursWidget *colours_widget;

    OutfitMod data; // Actual mod data
    QString location; // Active file path

    // Functions
    void save(bool as);
    void set_titles(const QString &title);
    void update_ui();
};

#endif // OUTFITWINDOW_H
