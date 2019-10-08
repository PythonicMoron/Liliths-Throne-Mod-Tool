#ifndef TAGSWIDGET_H
#define TAGSWIDGET_H

#include <QWidget>

// This class holds the logic for the tags widget.
// This might be replaced with something else. Not sure I like it.

namespace Ui {
class TagsWidget;
}

class TagsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TagsWidget(QStringList &tags, QWidget *parent);
    ~TagsWidget();

    // Functions
    static bool load_tags(bool force_internal = false);
    static const QStringList* get_tags();
    void open();
    void update();
    void reload_ui();

public slots:
    // "What's This?" function
    void whats_this();

protected:
    // Override closeEvent to intercept things like pressing the 'x' button.
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::TagsWidget *ui; // Ui
    QStringList *tags_ptr; // Pointer to tags list
    static QStringList tags; // List of tags used to populate list widget.

    // A function
    void apply();
};

#endif // TAGSWIDGET_H
