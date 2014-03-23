#ifndef ROBOTCONTROLPANEL_H
#define ROBOTCONTROLPANEL_H
#include <QDialog>
#include <QFileDialog>
#include "ui_controlPanel.h"
#include <QStringListModel>
class UsrAiNode;
class RobotModelCfg;
class QTime;
class RobotControlPanel:public QDialog, public Ui::controlPanel
{
    Q_OBJECT
public:
    RobotControlPanel(UsrAiNode *Node, RobotModelCfg *modelCfg, QWidget *parent = 0);
signals:
    void axisRotationChanged(double *m_rotationArray, int sizeofArray);
    void sigModelChanged();
    void sigInerSim();
public slots:
    void updateRobotNode(UsrAiNode *Node){ m_aiNode = Node; }
    void updateRobotCfgData(RobotModelCfg *modelCfg);
private slots:
    void on_SpinBox_S_valueChanged();
    void on_SpinBox_L_valueChanged();
    void on_SpinBox_U_valueChanged();
    void on_SpinBox_R_valueChanged();
    void on_SpinBox_B_valueChanged();
    void on_SpinBox_T_valueChanged();

    void on_listView_doubleClicked();
    void on_pushButton_editList_clicked();
    void on_pushButton_delList_clicked();
    void on_pushButton_addList_clicked();
    void on_pushButton_save_clicked();
    void on_pushButton_load_clicked();
    void on_toolButtonStart_clicked();
    void on_toolButtonStop_clicked();

    void upDateAxisesRotation(double angle, const QString &objname);
    void processInstructsStr(QString str);
    void simulateLists();
private:
    enum STATE{ IDLE, SHIELD_SPINBOX_SIG };
    QVector<QDoubleSpinBox *> m_spinBoxVector;
    QVector<QLabel *> m_labelVector;
    QStringListModel *teachingListModel;
    QStringList teachingList;
    UsrAiNode *m_aiNode;
    QString m_curFileName;
    RobotModelCfg *m_modelCfg;
    unsigned long m_simTimeIntetvalM;
    QTimer *m_innerTimer;
    STATE m_state;

};

#endif // ROBOTCONTROLPANEL_H
