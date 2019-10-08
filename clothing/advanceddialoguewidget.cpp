#include "advanceddialoguewidget.h"
#include "ui_advanceddialoguewidget.h"

#include <QShortcut>
#include <QStandardItemModel>
#include <QWhatsThis>
#include <QMenu>

#include "common/customcomboboxdelegate.h"

AdvancedDialogueWidget::AdvancedDialogueWidget(QMap<QString, QList<ClothingMod::XPlacementText>> &data, QWidget *parent) : RestrictedSlotEditorBase(parent), ui(new Ui::AdvancedDialogueWidget), map_data(&data)
{
    // Setup
    ui->setupUi(this);
    ui->typeList->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->typeList->setItemDelegate(new CustomComboBoxDelegate(access_types, this));
    ui->typeList->setFocusPolicy(Qt::NoFocus); // Prevents keyboard focus
    hide();

    // Create menu object
    menu = new QMenu(this);

    // Shortcuts
    new QShortcut(QKeySequence("F1"), this, SLOT(whats_this()));

    // Connections
    connect(parent->window(), &QWidget::windowTitleChanged, [this] (const QString &title) {setWindowTitle(title);});
    connect(ui->typeList, &QListView::pressed, [this] (const QModelIndex &index) {show_section(index.row());});
    connect(ui->typeList, &QListView::customContextMenuRequested, [this] (const QPoint &pos) {type_list_context_menu(pos);});
    connect(ui->saveButton, &QPushButton::pressed, [this] () {save_close();});
    connect(ui->discardButton, &QPushButton::pressed, [this] () {this->parentWidget()->window()->show(); this->hide();});
    connect(ui->whatsThisButton, &QPushButton::pressed, [this] () {whats_this();});
    connect(ui->selfTextEdit, &QPlainTextEdit::textChanged, [this] () {
        if (ui->typeList->currentIndex().isValid())
            current_item[ui->typeList->currentIndex().row()].self = ui->selfTextEdit->toPlainText();
    });
    connect(ui->otherTextEdit, &QPlainTextEdit::textChanged, [this] () {
        if (ui->typeList->currentIndex().isValid())
            current_item[ui->typeList->currentIndex().row()].other = ui->otherTextEdit->toPlainText();
    });
    connect(ui->roughTextEdit, &QPlainTextEdit::textChanged, [this] () {
        if (ui->typeList->currentIndex().isValid())
            current_item[ui->typeList->currentIndex().row()].other_rough = ui->roughTextEdit->toPlainText();
    });
    connect(ui->displacementButton, &QRadioButton::toggled, [this] (bool state) {
        if (ui->typeList->currentIndex().isValid()) {
            if (state)
                current_item[ui->typeList->currentIndex().row()].flag = ClothingMod::XPlacementText::displacement;
            else
                current_item[ui->typeList->currentIndex().row()].flag = ClothingMod::XPlacementText::replacement;
        }
    });
}

AdvancedDialogueWidget::~AdvancedDialogueWidget()
{
    // Clean up ui objects
    delete ui;
}

void AdvancedDialogueWidget::open_editor(const QString &slot)
{
    // Load data into ui and show window
    current_slot = slot;
    current_item = map_data->value(slot);

    // Display slot
    ui->slot_display_label->setText("Slot: " + slot);

    // Init typeList
    {
        // Delete old model
        QStandardItemModel *model = qobject_cast<QStandardItemModel*>(ui->typeList->model());
        if (model != nullptr)
            delete model;

        // Create new model and populate it
        model = new QStandardItemModel(ui->typeList);
        for (const ClothingMod::XPlacementText &text : current_item) {
            model->appendRow(new QStandardItem(text.type));
        }

        // Setup model/view
        ui->typeList->setModel(model);
        connect(model, &QStandardItemModel::itemChanged, [this] (QStandardItem *item) {type_list_modify_data(item->index());});
        if (model->rowCount() != 0)
            ui->typeList->setCurrentIndex(model->index(0,0));
    }

    // Init the rest of the ui if there is an item selected
    if (ui->typeList->currentIndex().isValid())
        show_section(ui->typeList->currentIndex().row());
    else {
        // Reset ui if there is no item selected
        ui->selfTextEdit->clear();
        ui->otherTextEdit->clear();
        ui->roughTextEdit->clear();
        ui->displacementButton->setChecked(true);
    }

    this->show();
}

void AdvancedDialogueWidget::setup(const QStringList &displacement_types)
{
    // Little function that sets the ui data. Called from ClothingWindow::load_defs
    access_types = displacement_types;
}

void AdvancedDialogueWidget::whats_this()
{
    // What do you think this does?
    QWhatsThis::enterWhatsThisMode();
}

void AdvancedDialogueWidget::save_close()
{
    // Apply data and swap windows

    map_data->insert(current_slot, current_item); // Overwrites old data
    this->parentWidget()->window()->show();
    this->hide();
}

void AdvancedDialogueWidget::show_section(int index)
{
    // Updates ui fields to match currently selected section

    // Set text fields
    ui->selfTextEdit->setPlainText(current_item[index].self);
    ui->otherTextEdit->setPlainText(current_item[index].other);
    ui->roughTextEdit->setPlainText(current_item[index].other_rough);

    // Set radio buttons
    if (current_item[index].flag == ClothingMod::XPlacementText::displacement) {
        ui->displacementButton->setChecked(true);
    } else
        ui->replacementButton->setChecked(true);
}

void AdvancedDialogueWidget::type_list_context_menu(const QPoint &pos)
{
    // Displays the context menu.

    // Empty the menu.
    menu->clear();

    // Obtain item index using a dark ritual.
    QModelIndex index = ui->typeList->indexAt(pos);

    // If a valid index is selected, add a remove action
    if (index.isValid()) {
        auto *rem_action = new QAction("Remove entry");
        QObject::connect(rem_action, &QAction::triggered, [this, index] () {type_list_remove_item(index);});
        menu->addAction(rem_action);
    }

    // Add an add action
    auto add_action = new QAction("Add entry");
    QObject::connect(add_action, &QAction::triggered, [this] () {type_list_add_item();});
    menu->addAction(add_action);

    // Show menu
    menu->popup(ui->typeList->viewport()->mapToGlobal(pos));
}

void AdvancedDialogueWidget::type_list_modify_data(const QModelIndex &index)
{
    // Modify data at index

    if (index.isValid())
        current_item[index.row()].type = index.data().toString();
}

void AdvancedDialogueWidget::type_list_remove_item(const QModelIndex &index)
{
    // Remove data at index

    if (index.isValid()) {
        current_item.removeAt(index.row());
        ui->typeList->model()->removeRow(index.row());

        // Update ui
        if (!current_item.isEmpty()) {
            // Select first item
            ui->typeList->setCurrentIndex(ui->typeList->model()->index(0, 0));
            show_section(0);
        } else {
            // Reset ui
            ui->selfTextEdit->clear();
            ui->otherTextEdit->clear();
            ui->roughTextEdit->clear();

            ui->displacementButton->blockSignals(true);
            ui->replacementButton->blockSignals(true);
            ui->displacementButton->setChecked(true);
            ui->displacementButton->blockSignals(false);
            ui->replacementButton->blockSignals(false);
        }
    }
}

void AdvancedDialogueWidget::type_list_add_item()
{
    // Add new item to data

    // Initialize new object
    ClothingMod::XPlacementText text;
    text.flag = ClothingMod::XPlacementText::displacement;
    text.type = "NULL";
    text.self = QString();
    text.other = QString();
    text.other_rough = QString();

    // Add new object
    current_item.append(text);
    qobject_cast<QStandardItemModel*>(ui->typeList->model())->appendRow(new QStandardItem(text.type));
}

QStringList AdvancedDialogueWidget::access_types = QStringList();
