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
#include <iostream>
#include "debug.h"
#include <QtOpenGL>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <queue>


UsrAiNode::UsrAiNode()
{
    mParent = NULL;
    mName = NULL;
    fileName = NULL;
    ishidden = false;
    nodeType = MODULE;
    m_angle = 0;
    mTransformation.identity();//init with identity
}

/** Construction from a specific name */
UsrAiNode::UsrAiNode(NodeType type, const char *name)
{ // set all members to zero by default
    mParent = NULL;
    mName = name;
    fileName = NULL;
    ishidden = false;
    nodeType = type;
    m_angle = 0;
    mTransformation.identity();//init with identity
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
UsrAiNode::~UsrAiNode() //当节点时，下挂的所有节点也均释放
{
    std::list<int>::iterator showlistiterator;
    std::list<UsrAiNode *>::iterator childrenItem;

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
            qDebug("delete showlist: %d,", *showlistiterator);
            glDeleteLists(*showlistiterator, 1);
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
UsrAiNode* UsrAiNode::FindNode(const char* name)
{
    std::list<UsrAiNode *>::iterator childrenItem;
    if (!::strcmp(mName,name))
    {
        return this;
    }

    for(childrenItem = childrenList.begin(); childrenItem != childrenList.end(); ++childrenItem)
    {
        UsrAiNode* p = (*childrenItem)->FindNode(name);
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
std::list<UsrAiNode *> UsrAiNode::getChildrenList(const char *objname)
{
    UsrAiNode * theFoundedNod = NULL;
    if (NULL != objname)
    {
        theFoundedNod = FindNode(objname);
        if (NULL == theFoundedNod)
        {
            WAR_OUT("%s,%d:node not exist",__FILE__,__LINE__);
            return (std::list<UsrAiNode *>)NULL;
        }
        return theFoundedNod->childrenList;
    }
    else
    {
        return this->childrenList;
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
const char * UsrAiNode::getmName(const char *objname)
{
    UsrAiNode * theFoundedNod = NULL;
    if (NULL != objname)
    {
        theFoundedNod = FindNode(objname);
        if (NULL == theFoundedNod)
        {
            WAR_OUT("%s,%d:node not exist",__FILE__,__LINE__);
            return NULL;
        }
        return theFoundedNod->mName;
    }
    else
    {
        return this->mName;
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
void UsrAiNode::addNodeToTree(UsrAiNode *Node, const char *objname)
{
    UsrAiNode * theFoundedNod = NULL;
    if (NULL != objname)
    {
        theFoundedNod = FindNode(objname);
        if (NULL == theFoundedNod)
        {
            WAR_OUT("%s,%d:node not exist",__FILE__,__LINE__);
            return ;
        }
        Node->mParent = theFoundedNod;
        theFoundedNod->childrenList.push_back(Node);
    }
    else
    {
        Node->mParent = this;
        this->childrenList.push_back(Node);
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
void UsrAiNode::addShowListToNode(int addlist,const char *objname)
{
    UsrAiNode * theFoundedNod = NULL;
    if (NULL != objname)
    {
        theFoundedNod = FindNode(objname);
        if (NULL == theFoundedNod)
        {
            WAR_OUT("%s,%d:node not exist",__FILE__,__LINE__);
            return ;
        }
        theFoundedNod->showList.push_back(addlist);
    }
    else
    {
        this->showList.push_back(addlist);
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
void UsrAiNode::setFileNameByNode(const char *nodeFileName, const char *objname)
{
    UsrAiNode * theFoundedNod = NULL;
    if (NULL != objname)
    {
        theFoundedNod = FindNode(objname);
        if (NULL == theFoundedNod)
        {
            WAR_OUT("%s,%d:node not exist",__FILE__,__LINE__);
            return ;
        }
        theFoundedNod->fileName = nodeFileName;
    }
    else
    {
        this->fileName = nodeFileName;
    }

}

/*************************************************
Function: // 函数名称
Description: // 函数功能、性能等的描述
    删除
Input: // 输入参数说明，包括每个参数的作
// 用、取值说明及参数间关系。
Output: // 对输出参数的说明。
Return: // 函数返回值的说明
Author: zhangjiankun
Others: // 其它说明
*************************************************/
void UsrAiNode::delNodeFromTree(const char *objname)
{
    UsrAiNode * theFoundedNod = NULL;
    if (NULL != objname)
    {
        theFoundedNod = FindNode(objname);
        if (NULL == theFoundedNod)
        {
            WAR_OUT("%s,%d:node not exist",__FILE__,__LINE__);
            return ;
        }
        delete theFoundedNod;
    }
    else
    {
        WAR_OUT("Input params eror!");
        return;
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
void UsrAiNode::rmShowList(int showlist, const char *objname)
{
    UsrAiNode * theFoundedNod = NULL;
    if (NULL != objname)
    {
        theFoundedNod = FindNode(objname);
        if (NULL == theFoundedNod)
        {
            WAR_OUT("%s,%d:node not exist",__FILE__,__LINE__);
            return ;
        }
        theFoundedNod->showList.remove(showlist);
    }
    else
    {
        this->showList.remove(showlist);
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
void UsrAiNode::callShowList(NodeType type)
{
    std::list<int>::iterator showlistiterator;
    std::list<UsrAiNode *>::iterator childrenItem;
    QTime t;
    t.start();

    glMultMatrixf(mTransformation.getTranspose());
    if (!showList.empty())
    {
        for (showlistiterator=showList.begin();showlistiterator!=showList.end(); ++showlistiterator)
        {
            DEBUG_OUT("call show list:%d",*showlistiterator);
            if (false == ishidden && type == nodeType)
            {
                  glCallList(*showlistiterator);
            }
        }
    }

    if (!childrenList.empty())
    {
        for(childrenItem = childrenList.begin(); childrenItem != childrenList.end(); ++childrenItem)
        {
            glPushMatrix();
            //在绘制每个物体前应该把当前状态先保存到堆栈中，再等绘制完后取出原来的状态
            glPushAttrib(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_CURRENT_BIT);
            (*childrenItem)->callShowList(type);
            glPopMatrix();
            glPopAttrib();
        }
    }

    //DEBUG_OUT("callShowList node %s.Time elapsed: %d ms",mName, t.elapsed());
}

/*************************************************
Function: // 函数名称
Description: // 函数功能、性能等的描述
    层序打印所有结点。
Input: // 输入参数说明，包括每个参数的作
// 用、取值说明及参数间关系。
Output: // 对输出参数的说明。
Return: // 函数返回值的说明
Author: zhangjiankun
Others: // 其它说明
*************************************************/
void UsrAiNode::printAllNode()
{
    UsrAiNode* positionInQue = NULL;
    std::queue<UsrAiNode *> aiNodeQue;
    std::list<int>::iterator showlistiterator;
    std::list<UsrAiNode *>::iterator childrenItem;

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
void UsrAiNode::setTranslationMatrix(const Matrix4 &m, const char *objname)
{
    UsrAiNode * theFoundedNod = NULL;
    if (NULL != objname)
    {
        theFoundedNod = FindNode(objname);
        if (NULL == theFoundedNod)
        {
            WAR_OUT("%s,%d:node not exist",__FILE__,__LINE__);
            return;
        }
        theFoundedNod->mTransformation = m;
    }
    else
    {
        this->mTransformation = m;
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
bool UsrAiNode::getHidden(const char *objname)
{
    UsrAiNode * theFoundedNod = NULL;
    if (NULL != objname)
    {
        theFoundedNod = FindNode(objname);
        if (NULL == theFoundedNod)
        {
            WAR_OUT("%s,%d:node not exist",__FILE__,__LINE__);
            return false;
        }
        return theFoundedNod->ishidden;
    }
    else
    {
        return this->ishidden;
    }
}

/*************************************************
Function: // 函数名称
Description: // 函数功能、性能等的描述
    找到节点inobjname，并执行x,y,z方向的位移。
Input: // 输入参数说明，包括每个参数的作
// 用、取值说明及参数间关系。
Output: // 对输出参数的说明。
Return: // 函数返回值的说明
Author: zhangjiankun
Others: // 其它说明
*************************************************/
void UsrAiNode::translateXYZ(const char *objname, float xposition, float yposition, float zposition)
{
     UsrAiNode * theFoundedNod = NULL;
     if (NULL != objname)
     {
         theFoundedNod = FindNode(objname);
         if (NULL == theFoundedNod)
         {
             WAR_OUT("%s,%d:node not exist",__FILE__,__LINE__);
             return;
         }
         theFoundedNod->mTransformation.translate(xposition, yposition, zposition);
     }
     else
     {
         this->mTransformation.translate(xposition, yposition, zposition);
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
void UsrAiNode::setHiddenFlagByName(bool hiddenFlag, const char *objname)
{
    UsrAiNode * theFoundedNod = NULL;
    if (NULL != objname)
    {
        theFoundedNod = FindNode(objname);
        if (NULL == theFoundedNod)
        {
            WAR_OUT("%s,%d:node not exist",__FILE__,__LINE__);
            return;
        }
        theFoundedNod->ishidden = hiddenFlag;
    }
    else
    {
        this->ishidden = hiddenFlag;
    }
}

/*************************************************
Function: // 函数名称
Description: // 函数功能、性能等的描述
    找到节点inobjname，并执行x,y,z方向的位移。
Input: // 输入参数说明，包括每个参数的作
// 用、取值说明及参数间关系。
Output: // 对输出参数的说明。
Return: // 函数返回值的说明
Author: zhangjiankun
Others: // 其它说明
*************************************************/
void UsrAiNode::setHiddenFlagRecs(bool hiddenflag)
{
    UsrAiNode* positionInQue = NULL;
    std::queue<UsrAiNode *> aiNodeQue;
    std::list<UsrAiNode *>::iterator childrenItem;
    std::list<UsrAiNode *> tmpChildList;

    //层序遍历
    aiNodeQue.push(this);

    while(!aiNodeQue.empty())
    {
        positionInQue = aiNodeQue.front();
        aiNodeQue.pop();

        //action begin
        positionInQue->ishidden = hiddenflag;
        //action end

        tmpChildList = positionInQue->getChildrenList();
        for(childrenItem = tmpChildList.begin(); childrenItem != tmpChildList.end(); ++childrenItem)
        {
            aiNodeQue.push(*childrenItem);
        }

    }
}

/*************************************************
Function: // 函数名称
Description: // 函数功能、性能等的描述
    找到节点inobjname，并执行x,y,z方向的位移。
Input: // 输入参数说明，包括每个参数的作
// 用、取值说明及参数间关系。
Output: // 对输出参数的说明。
Return: // 函数返回值的说明
Author: zhangjiankun
Others: // 其它说明
*************************************************/
void UsrAiNode::RotationXYZ(float angle, float x, float y, float z,const char *objname)
{
    UsrAiNode * theFoundedNod = NULL;
    if (NULL != objname)
    {
        theFoundedNod = FindNode(objname);
        if (NULL == theFoundedNod)
        {
            WAR_OUT("%s,%d:node not exist",__FILE__,__LINE__);
            return;
        }
        theFoundedNod->mTransformation.rotate(angle,x,y,z);
    }
    else
    {
        this->mTransformation.rotate(angle,x,y,z);
    }

}

/*************************************************
Function: // 函数名称
Description: // 函数功能、性能等的描述
    找到节点inobjname，并以x,y,z为转轴旋转angle度。
Input: // 输入参数说明，包括每个参数的作
// 用、取值说明及参数间关系。
Output: // 对输出参数的说明。
Return: // 函数返回值的说明
Author: zhangjiankun
Others: // 其它说明
*************************************************/
void UsrAiNode::setRotateXYZ(double angle, float x, float y, float z,const char *objname)
{
    UsrAiNode * theFoundedNod = NULL;
    if (NULL != objname)
    {
        theFoundedNod = FindNode(objname);
        if (NULL == theFoundedNod)
        {
            WAR_OUT("%s,%d:node not exist",__FILE__,__LINE__);
            return;
        }

        theFoundedNod->mTransformation.setRotation(angle,x,y,z);
    }
    else
    {
        DEBUG_OUT("%s,%d:objname %s",__FILE__,__LINE__,objname);

        this->mTransformation.setRotation(angle,x,y,z);
    }

}


/*************************************************
Function: // 函数名称
Description: // 函数功能、性能等的描述
    找到节点inobjname，并设置旋转轴。
Input: // 输入参数说明，包括每个参数的作
// 用、取值说明及参数间关系。
Output: // 对输出参数的说明。
Return: // 函数返回值的说明
Author: zhangjiankun
Others: // 其它说明
*************************************************/
void UsrAiNode::setRotationAxis(float x, float y, float z, const char *objname)
{
    UsrAiNode * theFoundedNod = NULL;
    if (NULL != objname)
    {
        theFoundedNod = FindNode(objname);
        if (NULL == theFoundedNod)
        {
            WAR_OUT("%s,%d:node not exist",__FILE__,__LINE__);
            return;
        }

        theFoundedNod->m_RotationAxis[0]=x;
        theFoundedNod->m_RotationAxis[1]=y;
        theFoundedNod->m_RotationAxis[2]=z;
    }
    else
    {
        DEBUG_OUT("%s,%d:objname %s",__FILE__,__LINE__,objname);
        m_RotationAxis[0]=x; m_RotationAxis[1]=y; m_RotationAxis[2]=z;
    }

}
void UsrAiNode::setAngle(double angle, const char *objname)
{
    UsrAiNode * theFoundedNod = NULL;
    if (NULL != objname)
    {
        theFoundedNod = FindNode(objname);
        if (NULL == theFoundedNod)
        {
            WAR_OUT("%s,%d:node not exist",__FILE__,__LINE__);
            return;
        }

        theFoundedNod->setAngle(angle);
    }
    else
    {
        DEBUG_OUT("%s,%d:objname %s",__FILE__,__LINE__,objname);
        setAngle(angle);
    }
}

void UsrAiNode::updateAngleToMat()
{
    UsrAiNode* positionInQue = NULL;
    std::queue<UsrAiNode *> aiNodeQue;
    std::list<UsrAiNode *>::iterator childrenItem;
    std::list<UsrAiNode *> tmpChildList;

    //层序遍历
    aiNodeQue.push(this);

    while(!aiNodeQue.empty())
    {
        positionInQue = aiNodeQue.front();
        aiNodeQue.pop();

        //action begin
        positionInQue->mTransformation.setRotation(positionInQue->m_angle,
            positionInQue->m_RotationAxis[0], positionInQue->m_RotationAxis[1], positionInQue->m_RotationAxis[2]);
        //action end

        tmpChildList = positionInQue->getChildrenList();
        for(childrenItem = tmpChildList.begin(); childrenItem != tmpChildList.end(); ++childrenItem)
        {
            aiNodeQue.push(*childrenItem);
        }

    }
}

/*************************************************
Function: // 函数名称
Description: // 函数功能、性能等的描述
    按照结点的转轴旋转angle。
Input: // 输入参数说明，包括每个参数的作
// 用、取值说明及参数间关系。
Output: // 对输出参数的说明。
Return: // 函数返回值的说明
Author: zhangjiankun
Others: // 其它说明
*************************************************/
void UsrAiNode::setRotateXYZ(double angle, const char *objname)
{
    UsrAiNode * theFoundedNod = NULL;
    float *rotationAxises = NULL;

    //查找当前结点下挂的名为objname的结点，绕该结点旋转angle。
    if (NULL != objname)
    {
        theFoundedNod = FindNode(objname);
        if (NULL == theFoundedNod)
        {
            WAR_OUT("%s,%d:node not exist",__FILE__,__LINE__);
            return;
        }
        rotationAxises = theFoundedNod->getRotationAxis();
        theFoundedNod->mTransformation.setRotation(angle, rotationAxises[0], rotationAxises[1], rotationAxises[2]);
    }
    else //默认绕当前结点的转轴，旋转angle。
    {
        DEBUG_OUT("%s,%d:objname %s",__FILE__,__LINE__,objname);
        rotationAxises = getRotationAxis();

        this->mTransformation.setRotation(angle, rotationAxises[0], rotationAxises[1], rotationAxises[2]);
    }
}

/*************************************************
Function: // 函数名称
Description: // 函数功能、性能等的描述
    层序打印所有结点。
Input: // 输入参数说明，包括每个参数的作
// 用、取值说明及参数间关系。
Output: // 对输出参数的说明。
Return: // 函数返回值的说明
Author: zhangjiankun
Others: // 其它说明
*************************************************/
void UsrAiNode::recureProcess( void *(*pFnProc)(UsrAiNode*), void(*pFnFinaleProc)(void *))
{
    UsrAiNode* positionInQue = NULL;
    std::queue<UsrAiNode *> aiNodeQue;
    std::list<UsrAiNode *>::iterator childrenItem;
    std::list<UsrAiNode *> tmpChildList;
    void * pResult = NULL;

    //入参检查
    if (NULL == pFnProc)
    {
        WAR_OUT("Input Param error!");
        return;
    }

    //层序遍历
    aiNodeQue.push(this);

    while(!aiNodeQue.empty())
    {
        positionInQue = aiNodeQue.front();
        aiNodeQue.pop();

        //action begin
        pResult = pFnProc(positionInQue);
        //action end

        tmpChildList = positionInQue->getChildrenList();
        for(childrenItem = tmpChildList.begin(); childrenItem != tmpChildList.end(); ++childrenItem)
        {
            aiNodeQue.push(*childrenItem);
        }

    }

    if (NULL != pFnFinaleProc)
    {
        pFnFinaleProc(pResult);
    }
}
