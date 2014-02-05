#include "position.h"
#include "ui_position.h"
#include "../usrainode.h"

#include <QStringListModel>

#include<iostream>
#include <queue>



position::position(usrAiNode *Node, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::position)
{
    root = Node;
    currentRootName = root->getmName();
    qDebug("position construct function %s", currentRootName);

    modelUpdate();

    ui->setupUi(this);
    ui->label_combox->setBuddy(ui->comboBox);
    ui->comboBox->setModel(nodeModel);
}

position::~position()
{
    delete ui;
}

void position::modelUpdate()
{
    QStringList items;
    usrAiNode* positionInQue = NULL;
    std::queue<usrAiNode *> aiNodeQue;
    std::list<usrAiNode *>::iterator childrenItem;
    std::list<usrAiNode *> tmpChildList;

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
