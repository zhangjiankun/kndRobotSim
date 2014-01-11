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
    void spin_change(double value);
private slots:
    void on_doubleSpinBox_valueChanged(double value);
};

#endif // ROBOTCONTROLPANEL_H
