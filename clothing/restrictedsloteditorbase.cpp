#include "restrictedsloteditorbase.h"

#include <QShortcut>
#include <QWhatsThis>
#include <QCloseEvent>

#include "notimplementedexception.h"
#include "utility.h"

RestrictedSlotEditorBase::RestrictedSlotEditorBase(QWidget *parent) : QWidget(parent)
{
    // Setup
    setWindowFlag(Qt::Window);
    hide();
}

void RestrictedSlotEditorBase::closeEvent(QCloseEvent *event)
{
    // Overrides the close event. Instead of closing the widget, we just hide it and show the parent.

    event->ignore();
    this->parentWidget()->window()->show();
    this->hide();
}

void RestrictedSlotEditorBase::open_editor(const QString& UNUSED(slot))
{
    // Implement me!
    throw NotImplementedException();
}
