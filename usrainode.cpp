/*************************************************
Copyright (C), 1988-1999, Tech. Co., Ltd.
File name: // 文件名
Author:
Version:
Date: // 作者、版本及完成日期
Description: // 用于详细说明此程序文件完成的主要功能，与其他模块
// 或函数的接口，输出值、取值范围、含义及参数间的控
// 制、顺序、独立或依赖等关系
Others: // 其它内容的说明
Function List: // 主要函数列表，每条记录应包括函数名及功能简要说明
1. ....
History: // 修改历史记录列表，每条修改记录应包括修改日期、修改
// 者及修改内容简述
1. Date:
Author:
Modification:
2. ...
*************************************************/
#include "usrainode.h"
#include<iostream>
#include<debug.h>
#include <QtOpenGL>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <queue>

/*************************************************
Function: // 函数名称
Description: // 函数功能、性能等的描述
Input: // 输入参数说明，包括每个参数的作
// 用、取值说明及参数间关系。
Output: // 对输出参数的说明。
Return: // 函数返回值的说明
Author: zhangjiankun
Others: // 其它说明
*************************************************/
usrAiNode::~usrAiNode() //当节点时，下挂的所有节点也均释放
{
    std::list<int>::iterator showlistiterator;
    std::list<usrAiNode *>::iterator childrenItem;

    if (!childrenList.empty())
    {
        for(childrenItem = childrenList.begin(); childrenItem != childrenList.end(); ++childrenItem)
        {
            delete *childrenItem;
        }
    }

    if (!showList.empty())
    {
        for (showlistiterator = showList.begin(); showlistiterator != showList.end(); ++showlistiterator)
        {
            DEBUG_OUT("%d,", *showlistiterator);
            //glDeleteLists(*showlistiterator, 1); showlist不能删掉，因为别的地方也有可能会用到
        }
    }
}

/*************************************************
Function: // 函数名称
Description: // 函数功能、性能等的描述
Input: // 输入参数说明，包括每个参数的作
// 用、取值说明及参数间关系。
Output: // 对输出参数的说明。
Return: // 函数返回值的说明
Author: zhangjiankun
Others: // 其它说明
*************************************************/
usrAiNode* usrAiNode::FindNode(const char* name)
{
    std::list<usrAiNode *>::iterator childrenItem;
    if (!::strcmp(mName,name))return this;
    for(childrenItem = childrenList.begin(); childrenItem != childrenList.end(); ++childrenItem)
    {
        usrAiNode* p = (*childrenItem)->FindNode(name);
        if (p)return p;
    }

    return NULL;
}

/*************************************************
Function: // 函数名称
Description: // 函数功能、性能等的描述
Input: // 输入参数说明，包括每个参数的作
// 用、取值说明及参数间关系。
Output: // 对输出参数的说明。
Return: // 函数返回值的说明
Author: zhangjiankun
Others: // 其它说明
*************************************************/
void usrAiNode::addNodeToTree(const char* objname, usrAiNode* Node)
{
    usrAiNode * theFoundedNod = FindNode(objname);
    if (NULL == theFoundedNod)
    {
        DEBUG_OUT("node not exist");
        return;
    }
    Node->mParent = theFoundedNod;
    theFoundedNod->childrenList.push_back(Node);
}

/*************************************************
Function: // 函数名称
Description: // 函数功能、性能等的描述
Input: // 输入参数说明，包括每个参数的作
// 用、取值说明及参数间关系。
Output: // 对输出参数的说明。
Return: // 函数返回值的说明
Author: zhangjiankun
Others: // 其它说明
*************************************************/
void usrAiNode::addShowListToNode(const char *objname,int addlist)
{
    usrAiNode * theFoundedNod = FindNode(objname);
    if (NULL == theFoundedNod)
    {
        DEBUG_OUT("%s,%d:node not exist",__FILE__,__LINE__);
        return;
    }
    theFoundedNod->showList.push_back(addlist);
}

/*************************************************
Function: // 函数名称
Description: // 函数功能、性能等的描述
Input: // 输入参数说明，包括每个参数的作
// 用、取值说明及参数间关系。
Output: // 对输出参数的说明。
Return: // 函数返回值的说明
Author: zhangjiankun
Others: // 其它说明
*************************************************/
void usrAiNode::addNodeFileToNode(const char *objname, const char *file_name)
{
    usrAiNode * theFoundedNod = FindNode(objname);
    if (NULL == theFoundedNod)
    {
        DEBUG_OUT("%s,%d:node not exist",__FILE__,__LINE__);
        return;
    }
    theFoundedNod->fileName = file_name;
}
#if 0
void usrAiNode::printShowListsFromRoot()
{
    std::list<int>::iterator showlistiterator;
    std::list<usrAiNode *>::iterator childrenitem;

    //print world root node's showLists
    for (showlistiterator=showList.begin();showlistiterator!=showList.end(); ++showlistiterator)
    {
        DEBUG_OUT("showlist:%d",*showlistiterator);
    }

    for (childrenitem = childrenList.begin(); childrenitem != childrenList.end(); ++childrenitem)
    {
        childrenitem->printShowListsFromRoot();
    }

    return;
}
#endif

/*************************************************
Function: // 函数名称
Description: // 函数功能、性能等的描述
Input: // 输入参数说明，包括每个参数的作
// 用、取值说明及参数间关系。
Output: // 对输出参数的说明。
Return: // 函数返回值的说明
Author: zhangjiankun
Others: // 其它说明
*************************************************/
void usrAiNode::delNodeFromTree(const char *objname, usrAiNode* Node)
{
    usrAiNode * theFoundedNod = FindNode(objname);
    if (NULL == theFoundedNod)
    {
        DEBUG_OUT("node not exist");
        return;
    }
    delete theFoundedNod;
}

/*************************************************
Function: // 函数名称
Description: // 函数功能、性能等的描述
Input: // 输入参数说明，包括每个参数的作
// 用、取值说明及参数间关系。
Output: // 对输出参数的说明。
Return: // 函数返回值的说明
Author: zhangjiankun
Others: // 其它说明
*************************************************/
void usrAiNode::rmShowList(const char *objname, int showlist)
{
    usrAiNode * theFoundedNod = FindNode(objname);
    if (NULL == theFoundedNod)
    {
        DEBUG_OUT("node not exist");
        return;
    }

    DEBUG_OUT("showlist removed:");
    theFoundedNod->showList.remove(showlist);
}

/*************************************************
Function: // 函数名称
Description: // 函数功能、性能等的描述
Input: // 输入参数说明，包括每个参数的作
// 用、取值说明及参数间关系。
Output: // 对输出参数的说明。
Return: // 函数返回值的说明
Author: zhangjiankun
Others: // 其它说明
*************************************************/
void usrAiNode::callShowList()
{
    std::list<int>::iterator showlistiterator;
    std::list<usrAiNode *>::iterator childrenItem;

    glMultMatrixf(mTransformation.getTranspose());
    for (showlistiterator=showList.begin();showlistiterator!=showList.end(); ++showlistiterator)
    {
      // dereference the iterator to get the element
        //DEBUG_OUT("call show list:%d",*showlistiterator);
        if (false == ishidden) {
            glCallList(*showlistiterator);
        }
    }

    for(childrenItem = childrenList.begin(); childrenItem != childrenList.end(); ++childrenItem)
    {
        glPushMatrix();
        //在绘制每个物体前应该把当前状态先保存到堆栈中，再等绘制完后取出原来的状态
        glPushAttrib(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_CURRENT_BIT);
        (*childrenItem)->callShowList();
        glPopMatrix();
        glPopAttrib();
    }
}

/*************************************************
Function: // 函数名称
Description: // 函数功能、性能等的描述
Input: // 输入参数说明，包括每个参数的作
// 用、取值说明及参数间关系。
Output: // 对输出参数的说明。
Return: // 函数返回值的说明
Author: zhangjiankun
Others: // 其它说明
*************************************************/
void usrAiNode::printAllNode()
{
    usrAiNode* positionInQue = NULL;
    std::queue<usrAiNode *> aiNodeQue;
    std::list<int>::iterator showlistiterator;
    std::list<usrAiNode *>::iterator childrenItem;

    std::cout<<"nodeName ";
    //层序遍历
    aiNodeQue.push(this);

    while(!aiNodeQue.empty())
    {
        positionInQue = aiNodeQue.front();
        aiNodeQue.pop();

        std::cout<<positionInQue->mName<<":";
        if(positionInQue->fileName != NULL)
            std::cout<<"("<<positionInQue->fileName<<") "<<std::endl;
        std::cout<<"showlist: ";
        if (!positionInQue->showList.empty())
        {
            for (showlistiterator=positionInQue->showList.begin();showlistiterator!=positionInQue->showList.end(); ++showlistiterator)
            {
                std::cout<<*showlistiterator<<" ";
            }
        }
        std::cout<<std::endl;

        for(childrenItem = positionInQue->childrenList.begin(); childrenItem != positionInQue->childrenList.end(); ++childrenItem)
        {
            aiNodeQue.push(*childrenItem);
        }

    }
    std::cout<<std::endl;
}


/*************************************************
Function: // 函数名称
Description: // 函数功能、性能等的描述
Input: // 输入参数说明，包括每个参数的作
// 用、取值说明及参数间关系。
Output: // 对输出参数的说明。
Return: // 函数返回值的说明
Author: zhangjiankun
Others: // 其它说明
*************************************************/
void usrAiNode::setTranslationMatrix(const char *objname, const Matrix4 & m)
{
    usrAiNode * theFoundedNod = FindNode(objname);
    if (NULL == theFoundedNod)
    {
        DEBUG_OUT("%s,%d:node not exist",__FILE__,__LINE__);
        return;
    }
    theFoundedNod->mTransformation = m;
}

/*************************************************
Function: // 函数名称
Description: // 函数功能、性能等的描述
Input: // 输入参数说明，包括每个参数的作
// 用、取值说明及参数间关系。
Output: // 对输出参数的说明。
Return: // 函数返回值的说明
Author: zhangjiankun
Others: // 其它说明
*************************************************/
void usrAiNode::setXTransition(float xposition)
{
    mTransformation.setPositionX(xposition);
}

/*************************************************
Function: // 函数名称
Description: // 函数功能、性能等的描述
Input: // 输入参数说明，包括每个参数的作
// 用、取值说明及参数间关系。
Output: // 对输出参数的说明。
Return: // 函数返回值的说明
Author: zhangjiankun
Others: // 其它说明
*************************************************/
void usrAiNode::setYTransition(float yposition)
{
    mTransformation.setPositionY(yposition);
    std::cout<<mTransformation<<std::endl;
}

/*************************************************
Function: // 函数名称
Description: // 函数功能、性能等的描述
Input: // 输入参数说明，包括每个参数的作
// 用、取值说明及参数间关系。
Output: // 对输出参数的说明。
Return: // 函数返回值的说明
Author: zhangjiankun
Others: // 其它说明
*************************************************/
void usrAiNode::setZTransition(float zposition)
{
    mTransformation.setPositionZ(zposition);
}

/*************************************************
Function: // 函数名称
Description: // 函数功能、性能等的描述
Input: // 输入参数说明，包括每个参数的作
// 用、取值说明及参数间关系。
Output: // 对输出参数的说明。
Return: // 函数返回值的说明
Author: zhangjiankun
Others: // 其它说明
*************************************************/
void usrAiNode::setXYZTransition(float xposition, float yposition, float zposition)
{
    mTransformation.translate(xposition, yposition, zposition);
}

/*************************************************
Function: // 函数名称
Description: // 函数功能、性能等的描述
Input: // 输入参数说明，包括每个参数的作
// 用、取值说明及参数间关系。
Output: // 对输出参数的说明。
Return: // 函数返回值的说明
Author: zhangjiankun
Others: // 其它说明
*************************************************/
void usrAiNode::setXRotation(int angle)
{
    mTransformation.rotateX(angle);
}

/*************************************************
Function: // 函数名称
Description: // 函数功能、性能等的描述
Input: // 输入参数说明，包括每个参数的作
// 用、取值说明及参数间关系。
Output: // 对输出参数的说明。
Return: // 函数返回值的说明
Author: zhangjiankun
Others: // 其它说明
*************************************************/
void usrAiNode::setYRotation(int angle)
{
    mTransformation.rotateY(angle);
}

/*************************************************
Function: // 函数名称
Description: // 函数功能、性能等的描述
Input: // 输入参数说明，包括每个参数的作
// 用、取值说明及参数间关系。
Output: // 对输出参数的说明。
Return: // 函数返回值的说明
Author: zhangjiankun
Others: // 其它说明
*************************************************/
void usrAiNode::setZRotation(int angle)
{
    mTransformation.rotateZ(angle);
}

