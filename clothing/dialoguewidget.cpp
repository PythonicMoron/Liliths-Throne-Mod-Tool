#include "dialoguewidget.h"
#include "ui_dialoguewidget.h"

DialogueWidget::DialogueWidget(QWidget *parent) : QWidget(parent), ui(new Ui::DialogueWidget), dialogue(nullptr)
{
    // Setup
    ui->setupUi(this);
    setWindowFlag(Qt::Window);
    setMinimumWidth(size().width());

    // Add some options to the flag combo box
    ui->flagComboBox->addItem("Displacement");
    ui->flagComboBox->addItem("Replacement");

    // Quick lambda to update current selection index text. Used in lambdas down in the connections.
    auto update_selection_entry = [this] (ClothingMod::XPlacementText &obj) {
        QString text;
        if (obj.flag == ClothingMod::XPlacementText::Flag::replacement)
            text = QString("Replacement, " + obj.type);
        else
            text = QString("Displacement, " + obj.type);

        ui->selctionComboBox->blockSignals(true);
        ui->selctionComboBox->setItemText(ui->selctionComboBox->currentIndex(), text);
        ui->selctionComboBox->blockSignals(false);
    };

    // Simple connections
    connect(ui->selctionComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [this] (int index) {open_section(index);});
    connect(ui->addButton, &QPushButton::released, [this] () {add_new();});
    connect(ui->removeButton, &QPushButton::released, [this] () {remove_current();});
    connect(ui->closeButton, &QPushButton::released, [this] () {parentWidget()->show(); hide();});

    // Not simple connections
    connect(ui->flagComboBox, QOverload<const QString &>::of(&QComboBox::currentIndexChanged), [this, update_selection_entry] (const QString &text) {
        if (ui->selctionComboBox->currentIndex() < 0)
            return;
        if (text == "Replacement")
            (*this->dialogue)[ui->selctionComboBox->currentIndex()].flag = ClothingMod::XPlacementText::Flag::replacement;
        else
            (*this->dialogue)[ui->selctionComboBox->currentIndex()].flag = ClothingMod::XPlacementText::Flag::displacement;
        update_selection_entry((*this->dialogue)[ui->selctionComboBox->currentIndex()]);
    });

    connect(ui->typeComboBox, QOverload<const QString &>::of(&QComboBox::currentIndexChanged), [this, update_selection_entry] (const QString &text) {
        if (ui->selctionComboBox->currentIndex() < 0 || text.isEmpty())
            return;
        (*this->dialogue)[ui->selctionComboBox->currentIndex()].type = text;
        update_selection_entry((*this->dialogue)[ui->selctionComboBox->currentIndex()]);
    });

    connect(ui->playerSelfEdit, &QPlainTextEdit::textChanged, [this] () {
        if (ui->selctionComboBox->currentIndex() < 0)
            return;
        (*this->dialogue)[ui->selctionComboBox->currentIndex()].player_self = ui->playerSelfEdit->toPlainText();
    });

    connect(ui->playerNpcEdit, &QPlainTextEdit::textChanged, [this] () {
        if (ui->selctionComboBox->currentIndex() < 0)
            return;
        (*this->dialogue)[ui->selctionComboBox->currentIndex()].player_npc = ui->playerNpcEdit->toPlainText();
    });

    connect(ui->playerNpcRoughEdit, &QPlainTextEdit::textChanged, [this] () {
        if (ui->selctionComboBox->currentIndex() < 0)
            return;
        (*this->dialogue)[ui->selctionComboBox->currentIndex()].player_npc_rough = ui->playerNpcRoughEdit->toPlainText();
    });

    connect(ui->npcSelfEdit, &QPlainTextEdit::textChanged, [this] () {
        if (ui->selctionComboBox->currentIndex() < 0)
            return;
        (*this->dialogue)[ui->selctionComboBox->currentIndex()].npc_self = ui->npcSelfEdit->toPlainText();
    });

    connect(ui->npcPlayerEdit, &QPlainTextEdit::textChanged, [this] () {
        if (ui->selctionComboBox->currentIndex() < 0)
            return;
        (*this->dialogue)[ui->selctionComboBox->currentIndex()].npc_player = ui->npcPlayerEdit->toPlainText();
    });

    connect(ui->npcPlayerRoughEdit, &QPlainTextEdit::textChanged, [this] () {
        if (ui->selctionComboBox->currentIndex() < 0)
            return;
        (*this->dialogue)[ui->selctionComboBox->currentIndex()].npc_player_rough = ui->npcPlayerRoughEdit->toPlainText();
    });

    connect(ui->npcNpcEdit, &QPlainTextEdit::textChanged, [this] () {
        if (ui->selctionComboBox->currentIndex() < 0)
            return;
        (*this->dialogue)[ui->selctionComboBox->currentIndex()].npc_other_npc = ui->npcNpcEdit->toPlainText();
    });

    connect(ui->npcNpcRoughEdit, &QPlainTextEdit::textChanged, [this] () {
        if (ui->selctionComboBox->currentIndex() < 0)
            return;
        (*this->dialogue)[ui->selctionComboBox->currentIndex()].npc_other_npc_rough = ui->npcNpcRoughEdit->toPlainText();
    });
}

DialogueWidget::~DialogueWidget()
{
    delete ui;
}

void DialogueWidget::setup(const QStringList &displacement_types)
{
    // Little function that sets the ui data. Called from ClothingWindow::load_defs
    access_types = displacement_types;
}

void DialogueWidget::open(QList<ClothingMod::XPlacementText> &dialogue)
{
    // Does some quick setup, hides the parent window, and then displays the widget.

    this->dialogue = &dialogue;

    reset_ui();

    if (!dialogue.isEmpty()) {
        open_section(0);
    }

    parentWidget()->hide();
    show();
}

void DialogueWidget::reload_ui()
{
    // This resets the type combo box and repopulates it. Called from ClothingWindow::populate_ui

    // Show parent (if needed) and hide widget
    if (!parentWidget()->isVisible())
        parentWidget()->show();
    hide();

    ui->typeComboBox->clear();
    ui->typeComboBox->addItems(access_types);
}

void DialogueWidget::closeEvent(QCloseEvent *event)
{
    // Overrides the close event. Instead of closing the widget, we just hide it and show the parent.

    event->ignore();
    parentWidget()->show();
    hide();
}

void DialogueWidget::reset_ui()
{
    // Does as it says it does.

    // Refresh contents of the [XPlacementText] selection box then reset it (with signal blocking to prevent triggering open_section)
    ui->selctionComboBox->blockSignals(true);
    ui->selctionComboBox->clear();
    QStringList entries = *new QStringList();
    for (ClothingMod::XPlacementText &obj : *dialogue) {
        QString text;
        if (obj.flag == ClothingMod::XPlacementText::Flag::replacement)
            text = QString("Replacement, " + obj.type);
        else
            text = QString("Displacement, " + obj.type);
        entries.append(text);
    }
    ui->selctionComboBox->addItems(entries);
    ui->typeComboBox->setCurrentIndex(-1);
    ui->selctionComboBox->blockSignals(false);

    // Reset indexes of the combo boxes
    ui->selctionComboBox->setCurrentIndex(-1);
    ui->flagComboBox->setCurrentIndex(-1);

    // Clear all text edit fields
    ui->playerSelfEdit->clear();
    ui->playerNpcEdit->clear();
    ui->playerNpcRoughEdit->clear();
    ui->npcSelfEdit->clear();
    ui->npcPlayerEdit->clear();
    ui->npcPlayerRoughEdit->clear();
    ui->npcNpcEdit->clear();
    ui->npcNpcRoughEdit->clear();
}

void DialogueWidget::remove_current()
{
    // Removes the current [XPlacementText] from data.

    int index = ui->selctionComboBox->currentIndex();
    if (index < 0)
        return;

    dialogue->removeAt(index);
    reset_ui();

    if (!dialogue->isEmpty())
        open_section(0);
}

void DialogueWidget::add_new()
{
    // Appends a new [XPlacementText] to data.

    dialogue->append(ClothingMod::XPlacementText());
    reset_ui();
    ui->selctionComboBox->setCurrentIndex(dialogue->count() - 1);
}

void DialogueWidget::open_section(int index)
{
    // Opens the [XPlacementText] at given index.

    if (index < 0)
        return;

    // Set selection combo box (with signal blocking to prevent looping)
    ui->selctionComboBox->blockSignals(true);
    ui->selctionComboBox->setCurrentIndex(index);
    ui->selctionComboBox->blockSignals(false);

    // Set flag combo box
    ClothingMod::XPlacementText *obj = &(*dialogue)[index];
    if (obj->flag == ClothingMod::XPlacementText::replacement)
        ui->flagComboBox->setCurrentIndex(ui->flagComboBox->findText("Replacement"));
    else
        ui->flagComboBox->setCurrentIndex(ui->flagComboBox->findText("Displacement"));

    // Set type combo box
    ui->typeComboBox->setCurrentIndex(ui->typeComboBox->findText(obj->type));

    // Fill out the text edit fields
    ui->playerSelfEdit->setPlainText(obj->player_self);
    ui->playerNpcEdit->setPlainText(obj->player_npc);
    ui->playerNpcRoughEdit->setPlainText(obj->player_npc_rough);
    ui->npcSelfEdit->setPlainText(obj->npc_self);
    ui->npcPlayerEdit->setPlainText(obj->npc_player);
    ui->npcPlayerRoughEdit->setPlainText(obj->npc_player_rough);
    ui->npcNpcEdit->setPlainText(obj->npc_other_npc);
    ui->npcNpcRoughEdit->setPlainText(obj->npc_other_npc_rough);
}

QStringList DialogueWidget::access_types = QStringList();
