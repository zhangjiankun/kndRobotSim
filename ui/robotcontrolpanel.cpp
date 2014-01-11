#include <QtGui>
#include "robotcontrolpanel.h"

robotcontrolpanel::robotcontrolpanel(QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);

    //connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
    //connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

}
void robotcontrolpanel::on_doubleSpinBox_valueChanged(double value)
{
    qDebug("%f",value);
    emit spin_change(value);
    //connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
    //connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

}

