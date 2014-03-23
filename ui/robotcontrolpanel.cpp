#include <QtGui>
#include <QStringListModel>
#include <QDebug>
#include <QTime>

#include "robotcontrolpanel.h"

#include "../usrainode.h"
#include "../robotmodelcfg.h"
#include "../instructpaser.h"



RobotControlPanel::RobotControlPanel(UsrAiNode *Node, RobotModelCfg * modelCfg, QWidget *parent)
    : QDialog(parent)
{
    m_aiNode = Node;
    setupUi(this);

    m_spinBoxVector<<SpinBox_S<<SpinBox_L<<SpinBox_U<<SpinBox_R<<SpinBox_B<<SpinBox_T;
    m_labelVector<<label<<label_2<<label_3<<label_4<<label_5<<label_6;

    updateRobotCfgData(modelCfg);

    teachingListModel = new QStringListModel;
    listView->setModel(teachingListModel);
    listView->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置不支持编辑列表

    m_simTimeIntetvalM = 1000;
    m_innerTimer = NULL;

    //初始状态为空闲状态。
    m_state = IDLE;

    toolButtonStart->setAutoRaise(true);
    toolButtonStop->setAutoRaise(true);

}

void RobotControlPanel::updateRobotCfgData(RobotModelCfg *modelCfg)
{
    if (modelCfg->getAxisNum() < m_spinBoxVector.count())
    {
        QMessageBox::information(this, tr("Robot Model"),
                             tr("Control Panel is not suitable for this model!"));
        return;
    }

    for (int i = 0; i < m_spinBoxVector.count(); i++)
    {
        m_spinBoxVector[i]->setRange(modelCfg->get_axisRotAttr(i+1)[0], modelCfg->get_axisRotAttr(i+1)[1]);
        m_spinBoxVector[i]->setAccessibleName(modelCfg->get_nodeAxisName(i+1));
        m_labelVector[i]->setText(modelCfg->get_nodeAxisName(i+1));//更新Label显示。
    }

}

void RobotControlPanel::on_SpinBox_S_valueChanged()
{
    upDateAxisesRotation(SpinBox_S->value(), SpinBox_S->accessibleName());
    if (SHIELD_SPINBOX_SIG != m_state)
    {
        emit sigModelChanged();
    }
}
void RobotControlPanel::on_SpinBox_L_valueChanged()
{
    upDateAxisesRotation(SpinBox_L->value(), SpinBox_L->accessibleName());
    if (SHIELD_SPINBOX_SIG != m_state)
    {
        emit sigModelChanged();
    }
}
void RobotControlPanel::on_SpinBox_U_valueChanged()
{
    upDateAxisesRotation(SpinBox_U->value(), SpinBox_U->accessibleName());
    if (SHIELD_SPINBOX_SIG != m_state)
    {
        emit sigModelChanged();
    }
}
void RobotControlPanel::on_SpinBox_R_valueChanged()
{
    upDateAxisesRotation(SpinBox_R->value(), SpinBox_R->accessibleName());
    if (SHIELD_SPINBOX_SIG != m_state)
    {
        emit sigModelChanged();
    }
}
void RobotControlPanel::on_SpinBox_B_valueChanged()
{
    upDateAxisesRotation(SpinBox_B->value(), SpinBox_B->accessibleName());
    if (SHIELD_SPINBOX_SIG != m_state)
    {
        emit sigModelChanged();
    }
}
void RobotControlPanel::on_SpinBox_T_valueChanged()
{
    upDateAxisesRotation(SpinBox_T->value(), SpinBox_T->accessibleName());
    if (SHIELD_SPINBOX_SIG != m_state)
    {
        emit sigModelChanged();
    }
}

void RobotControlPanel::processInstructsStr(QString str)
{
    if (str.isEmpty())
    {
        return;
    }

    str.trimmed();//去掉首尾空白
    str.simplified();//合并中间空格
    QStringList datas = str.split(" ");

    //设置结点的转过的角度值
    if(m_spinBoxVector.count() != datas.length())
    {
        return;
    }
    unsigned i = 0;
    foreach(QString dataStr, datas)
    {
        double angle = dataStr.toDouble();
        QDoubleSpinBox *spinBox = m_spinBoxVector[i];
        spinBox->setValue(angle); //更新到面板spinbox
        i++;
        m_aiNode->setAngle(angle, qPrintable(spinBox->accessibleName()));//更新到结点树
    }

    //更新各个结点到该结点的旋转矩阵。
    m_aiNode->updateAngleToMat();
}

void RobotControlPanel::on_listView_doubleClicked()
{
    QVariant variant = teachingListModel->data(listView->currentIndex(), Qt::DisplayRole);
    QString str = variant.value<QString>();
    m_state = SHIELD_SPINBOX_SIG;
    processInstructsStr(str);
    m_state = IDLE;
    emit sigModelChanged();
}

void RobotControlPanel::on_pushButton_editList_clicked()
{
    int row = listView->currentIndex().row();
    QModelIndex index = teachingListModel->index(row);
    teachingListModel->setData(index, "aaaa");
    listView->setCurrentIndex(index);
    listView->edit(index);
}

void RobotControlPanel::on_pushButton_delList_clicked()
{
    teachingListModel->removeRows(listView->currentIndex().row(), 1);
}

void RobotControlPanel::on_pushButton_addList_clicked()
{
    //当前位置后面插入指令
    int row = listView->currentIndex().row() + 1;
    teachingListModel->insertRows(row, 1);

    //生成指令
    QString str;
    for (int i =0; i < m_spinBoxVector.count(); i++)
    {
        str.append(QString::number(m_spinBoxVector[i]->value()));
        str.append("  ");
    }

    QModelIndex index = teachingListModel->index(row);
    teachingListModel->setData(index, str);

    //选中当前指令
    listView->setCurrentIndex(index);

}

void RobotControlPanel::on_toolButtonStart_clicked()
{
    //移动到第一条指令。
    listView->setCurrentIndex(teachingListModel->index(0));

    if (NULL == m_innerTimer)
    {
        m_innerTimer = new QTimer(this);

        m_state = SHIELD_SPINBOX_SIG;
        toolButtonStart->setAutoRaise(false);

        connect(m_innerTimer, SIGNAL(timeout()), this, SLOT(simulateLists()));
        m_innerTimer->start(m_simTimeIntetvalM);

        qDebug("simulation start");
    }
}

void RobotControlPanel::on_toolButtonStop_clicked()
{
    if (NULL != m_innerTimer)
    {
        disconnect(m_innerTimer, SIGNAL(timeout()), this, SLOT(simulateLists()));
        delete m_innerTimer;
        m_innerTimer = NULL;

        m_state = IDLE;
        toolButtonStart->setAutoRaise(true);
        qDebug("simulate stop");
    }
}

void RobotControlPanel::simulateLists()
{
    //1.获取的当前list 的指令
    int row = listView->currentIndex().row();
    QModelIndex index = teachingListModel->index(row);
    QVariant variant = teachingListModel->data(index, Qt::DisplayRole);
    QString str = variant.value<QString>();
    processInstructsStr(str);
    emit sigModelChanged();

    //2.移动到下一条指令
    listView->setCurrentIndex(teachingListModel->index(row + 1));

    //3.如果时最后一条指令，取消定时调用。
    int maxRow = teachingListModel->rowCount();
    if (row >= maxRow - 1)
    {
        disconnect(m_innerTimer, SIGNAL(timeout()), this, SLOT(simulateLists()));
        delete m_innerTimer;
        m_innerTimer = NULL;

        m_state = IDLE;
        toolButtonStart->setAutoRaise(true);
        qDebug("simulate stop");

    }

}

void RobotControlPanel::upDateAxisesRotation(double angle, const QString &objname)
{
    if (NULL == m_aiNode )
    {
        qWarning("%s,%d:uncorrect m_rotationArray", __FILE__, __LINE__);
        return;
    }

    m_aiNode->setAngle(angle, qPrintable(objname));
    m_aiNode->updateAngleToMat();

}

void RobotControlPanel::on_pushButton_load_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                               tr("Save teaching lists to file"), ".",
                               tr("Teaching lists files (*.txt)"));

    if (fileName.isEmpty())
    {
        qDebug("Filename is null %s",qPrintable(fileName));
        return;
    }

    m_curFileName = fileName;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadWrite))
    {
        QMessageBox::warning(this, tr("Spreadsheet"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(file.fileName())
                             .arg(file.errorString()));
        return ;
    }


    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine();
        teachingList += line;
    }

    teachingListModel->setStringList(teachingList);
}


void RobotControlPanel::on_pushButton_save_clicked()
{
    QString fileName;

    if (m_curFileName.isEmpty())
    {
        fileName = QFileDialog::getSaveFileName(this,
                                   tr("Save teaching lists to file"), ".",
                                   tr("Teaching lists files (*.txt)"));
        if (fileName.isEmpty())
        {
            qDebug("Filename is null %s",qPrintable(fileName));
            return;
        }
        m_curFileName = fileName;
    }
    else
    {
        fileName = m_curFileName;
    }


    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::warning(this, tr("Spreadsheet"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(file.fileName())
                             .arg(file.errorString()));
        return ;
    }


    QTextStream out(&file);

    foreach(QString stringItem, teachingListModel->stringList())
    {
        out<<stringItem<<endl;
    }

    //事实上，file离开作用域之后，便会自动关闭。
    file.close();
}
