#include "tagswidget.h"
#include "ui_tagswidget.h"

#include <QCloseEvent>
#include <QJsonDocument>
#include <QJsonArray>

#include "utility.h"

TagsWidget::TagsWidget(QStringList &tags, QWidget *parent) : QWidget(parent), ui(new Ui::TagsWidget), tags_ptr(&tags)
{
    // Setup
    ui->setupUi(this);
    setWindowFlag(Qt::Window);
    ui->listWidget->setSelectionMode(QAbstractItemView::MultiSelection);
    hide();

    // Connect
    connect(ui->doneButton, &QPushButton::released, [this] () {apply();});
}

TagsWidget::~TagsWidget()
{
    delete ui;
}

bool TagsWidget::load_tags(bool force_internal)
{
    // This function loads the data from the tags json file and replaces the static ui data.

    // Returns succuss state (only returns false if both internal and external files failed to load).
    // The force_internal parameter tells the function to ignore external files, if any, when set to true. Defaults to false.

    QFile file("item_tags.json"); // First, check for external file. Allows for overriding internal resource.
    if (!file.exists() || force_internal) {
        file.setFileName(":/res/data_files/item_tags.json"); // If no external file exists or this was called with force_internal set to true, load internal file.
        if (!file.exists()) {
            Utility::error("Could not open a valid item_tags.json file!"); // Something or someone really fucked up.
            return false;
        }
    }

    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString data = file.readAll();
    file.close();
    QJsonDocument json_tags = QJsonDocument::fromJson(data.toUtf8());

    // The internal file is compiled in and unable to be edited, so it should always be valid outside of development testing. Thus, it is assumed that all errors from here on are the fault of an external file.
    if (json_tags.isNull() || !json_tags.isObject() || !json_tags["tags"].isArray()) {
        if (force_internal) // This check helps prevent infinite looping during development or in the event something goes really, really wrong.
            return false;
        Utility::error("External tags.json file failed structure check! File will be ignored.");
        return load_tags(true);
    }

    // Get tags.
    for (QJsonValue item : json_tags["tags"].toArray())
        if(!item.isString())
            Utility::error("Bad item tag data. Entry will be skipped.");
        else
            tags.append(item.toString());
    tags.sort();

    return true;
}

void TagsWidget::open()
{
    // Calls update, then swaps visibility for this widget and the parent window.

    update();
    show();
    parentWidget()->hide();
}

void TagsWidget::update()
{
    // Updates the selected items in the widget to those in the data.

    ui->listWidget->clearSelection();
    for (const QString &tag : *tags_ptr)
        for (int i = 0; i < ui->listWidget->count(); i++)
            if (tag == ui->listWidget->item(i)->text()) {
                ui->listWidget->item(i)->setSelected(true);
                break;
            }
}

void TagsWidget::reload_ui()
{
    // Repopulates the list widget. Also hides widget and shows parent window.

    // Show parent (if needed) and hide widget
    if (!parentWidget()->isVisible())
        parentWidget()->show();
    hide();

    ui->listWidget->clear();
    ui->listWidget->addItems(tags);
}

void TagsWidget::closeEvent(QCloseEvent *event)
{
    // Overrides the close event. Instead of closing the widget, we just hide it and show the parent.

    event->ignore();
    parentWidget()->show();
    hide();
}

void TagsWidget::apply()
{
    // Applies selected tags to data

    tags_ptr->clear();
    if (!ui->listWidget->selectedItems().empty()) {
        QStringList tags = QStringList();
        for (QListWidgetItem *item : ui->listWidget->selectedItems())
            tags.append(item->text());
        tags_ptr->append(tags);
    }

    parentWidget()->show();
    hide();
}

// Static vars
QStringList TagsWidget::tags = QStringList();
