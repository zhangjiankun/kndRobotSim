#include "position.h"
#include "ui_position.h"
#include "../usrainode.h"
#include "debug.h"

#include <QStringListModel>

#include<iostream>
#include <queue>



position::position(UsrAiNode *Node, QWidget *parent) : //position 构建时，必须指定根节点。
    QDialog(parent),
    ui(new Ui::position)
{
    root = Node;
    currentRootName = root->getmName();
    qDebug("position construct function %s", currentRootName);

    modelUpdate();//生成模型。

    ui->setupUi(this);
    ui->label_combox->setBuddy(ui->comboBox);
    ui->comboBox->setModel(nodeModel);//将模型绑定到下拉列表。
}

position::~position()
{
    delete ui;
}

void position::modelUpdate()
{
    QStringList items;
    UsrAiNode* positionInQue = NULL;
    std::queue<UsrAiNode *> aiNodeQue;
    std::list<UsrAiNode *>::iterator childrenItem;
    std::list<UsrAiNode *> tmpChildList;

    if (NULL == root)
        return;

    //层序遍历
    aiNodeQue.push(root);

    while(!aiNodeQue.empty())
    {
        positionInQue = aiNodeQue.front();
        aiNodeQue.pop();

        //action begin
        items<<tr(positionInQue->getmName());
        //action end

        tmpChildList = positionInQue->getChildrenList();
        for(childrenItem = tmpChildList.begin(); childrenItem != tmpChildList.end(); ++childrenItem)
        {
            aiNodeQue.push(*childrenItem);
        }

    }

    nodeModel = new QStringListModel(items, this);
}

void position::setXTransition(double value)
{
    ui->xPosition->setValue(value);
}

void position::setYTransition(double value)
{
    ui->yPosition->setValue(value);
}

void position::setZTransition(double value)
{
    ui->zPosition->setValue(value);
}

void position::on_comboBox_currentIndexChanged(const QString & text)  //下拉列表变化时，触发的设置spin的值。
{
    UsrAiNode * theFoundedNod = NULL;
    globel = text;

    if (NULL != root) {
        theFoundedNod = root->FindNode(qPrintable(text));
        if (NULL == theFoundedNod) {
            DEBUG_OUT("%s,%d:node not exist",__FILE__,__LINE__);
            return;
        }
        setXTransition(theFoundedNod->getXPosition());
        setYTransition(theFoundedNod->getYPosition());
        setZTransition(theFoundedNod->getZPosition());
    }
}
