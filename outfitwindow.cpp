#include "outfitwindow.h"
#include "ui_outfitwindow.h"

OutfitWindow::OutfitWindow(const QDomDocument &xml_doc, const QString &path, QWidget *parent) : QWidget(parent), ui(new Ui::OutfitWindow)
{
    ui->setupUi(this);
}

OutfitWindow::~OutfitWindow()
{
    delete ui;
}
