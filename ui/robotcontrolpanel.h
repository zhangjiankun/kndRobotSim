#ifndef ROBOTCONTROLPANEL_H
#define ROBOTCONTROLPANEL_H
#include <QDialog>
#include "ui_controlPanel.h"
class robotcontrolpanel:public QDialog, public Ui::controlPanel
{
    Q_OBJECT
public:
    robotcontrolpanel(QWidget *parent);
signals:
    void axisRotationChanged(double *rotationArray, int sizeofArray);
private slots:
    void on_SpinBox_S_valueChanged(){
        rotationArray[S] = SpinBox_S->value();;
        emit axisRotationChanged(rotationArray, sizeof(rotationArray)/sizeof(double));
    }
    void on_SpinBox_L_valueChanged(){
        rotationArray[L] = SpinBox_L->value();
        emit axisRotationChanged(rotationArray, sizeof(rotationArray)/sizeof(double));
    }
    void on_SpinBox_U_valueChanged(){
        rotationArray[U] = SpinBox_U->value();;
        emit axisRotationChanged(rotationArray, sizeof(rotationArray)/sizeof(double));
    }
    void on_SpinBox_R_valueChanged(){
        rotationArray[R] = SpinBox_R->value();;
        emit axisRotationChanged(rotationArray, sizeof(rotationArray)/sizeof(double));
    }
    void on_SpinBox_B_valueChanged(){
        rotationArray[B] = SpinBox_B->value();;
        emit axisRotationChanged(rotationArray, sizeof(rotationArray)/sizeof(double));
    }
    void on_SpinBox_T_valueChanged(){
        rotationArray[T] = SpinBox_T->value();;
        emit axisRotationChanged(rotationArray, sizeof(rotationArray)/sizeof(double));
    }
private:
    enum axises{ S, L, U, R, B, T,AXIES_END};
    double rotationArray[AXIES_END];//enum axises
};

#endif // ROBOTCONTROLPANEL_H
