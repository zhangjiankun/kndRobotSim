#include "modeltree.h"
#include "ui_modeltree.h"

modelTree::modelTree(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::modelTree)
{
    ui->setupUi(this);
}

modelTree::~modelTree()
{
    delete ui;
}
