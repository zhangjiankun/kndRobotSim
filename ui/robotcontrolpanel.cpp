#include <QtGui>
#include "robotcontrolpanel.h"

robotcontrolpanel::robotcontrolpanel(QWidget *parent)
    : QDialog(parent)
{
    for (unsigned int i = 0; i < sizeof(rotationArray)/sizeof(double); i++){
        rotationArray[i] = 0.f;
    }
    setupUi(this);

    //connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
    //connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

}


