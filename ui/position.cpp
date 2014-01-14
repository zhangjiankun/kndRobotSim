#include "position.h"
#include "ui_position.h"

position::position(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::position)
{
    ui->setupUi(this);
}

position::~position()
{
    delete ui;
}
