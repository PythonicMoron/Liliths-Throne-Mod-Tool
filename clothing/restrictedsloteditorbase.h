#ifndef RESTRICTEDSLOTEDITORBASE_H
#define RESTRICTEDSLOTEDITORBASE_H

#include <QWidget>

class RestrictedSlotEditorBase : public QWidget
{
public:
    RestrictedSlotEditorBase(QWidget *parent);

    // Functions
    virtual void open_editor(const QString &slot) = 0;

protected:
    // Override closeEvent to intercept things like pressing the 'x' button.
    void closeEvent(QCloseEvent *event) override;

    // Data
    QString current_slot;
};

#endif // RESTRICTEDSLOTEDITORBASE_H
