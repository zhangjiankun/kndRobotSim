#include "usrainode.h"
#include<iostream>
#include<debug.h>
#include <QtOpenGL>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <queue>

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

void usrAiNode::callShowList()
{
    std::list<int>::iterator showlistiterator;
    std::list<usrAiNode *>::iterator childrenItem;


    mTransformation.Transpose();
    glMultMatrixf((float*)&mTransformation);
    for (showlistiterator=showList.begin();showlistiterator!=showList.end(); ++showlistiterator)
    {
      // dereference the iterator to get the element
        DEBUG_OUT("call show list:%d",*showlistiterator);
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

void usrAiNode::setTranslationMatrix(const char *objname, const aiMatrix4x4& m)
{
    usrAiNode * theFoundedNod = FindNode(objname);
    if (NULL == theFoundedNod)
    {
        DEBUG_OUT("%s,%d:node not exist",__FILE__,__LINE__);
        return;
    }
    theFoundedNod->mTransformation = m;
}

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




