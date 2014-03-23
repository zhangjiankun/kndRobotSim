#ifndef USRAINODE_H
#define USRAINODE_H
//#include"assimp/scene.h" 使用新的数据结构 Matrix4。
#include "Matrices.h"
#include<list>
class UsrAiNode
{
public:
    enum NodeType { AXIS, MODULE };
    UsrAiNode();

    //name 的生存周期需要和本类一致。
    UsrAiNode(NodeType type, const char *name);

    //释放该结点下挂的所有结点，以及每个结点上的显示列表。
    ~UsrAiNode();

    //以下函数只处理单个结点
    inline std::list<UsrAiNode *> getChildrenList() {return childrenList;}
    std::list<UsrAiNode *> getChildrenList(const char *objname);

    inline const char * getmName() { return mName;}
    const char * getmName(const char *objname);

    void addNodeToTree(UsrAiNode* Node, const char *objname = NULL);
    void delNodeFromTree(const char *objname);

    void setFileNameByNode(const char *nodeFileName, const char *objname = NULL);

    void addShowListToNode(int addlist, const char *objname = NULL);
    void rmShowList(int showlist, const char *objname = NULL);

    void setTranslationMatrix(const Matrix4& m, const char *objname = NULL);

    bool getHidden(const char *objname);
    void setHiddenFlagByName(bool hiddenFlag, const char *objname = NULL);

    //以下函数递归处理所有结点
    void callShowList(NodeType type = MODULE);//递归显示以当前结点为根结点的所有结点的显示列表。
    void printAllNode(); //递归显示所有结点的显示列表
    void setHiddenFlagRecs(bool hiddenflag);

    UsrAiNode* FindNode(const char* name);//
public:
    void setXTransition(float xposition) { mTransformation.setPositionX(xposition); }
    void setYTransition(float yposition) { mTransformation.setPositionY(yposition); }
    void setZTransition(float zposition) { mTransformation.setPositionZ(zposition); }
    void translateXYZ(const char *objname, float xposition, float yposition, float zposition);

    float getXPosition(){ return mTransformation.getPositionX(); }
    float getYPosition(){ return mTransformation.getPositionY(); }
    float getZPosition(){ return mTransformation.getPositionZ(); }

    void RotationX(float angle) { mTransformation.rotateX(angle); }
    void RotationY(float angle) { mTransformation.rotateY(angle); }
    void RotationZ(float angle) { mTransformation.rotateZ(angle); }
    void RotationXYZ(float angle, float x, float y, float z,const char *objname = NULL);
    void RotationXYZ(const char *objname,float angle, float x, float y, float z)
    {  RotationXYZ( angle,  x,  y,  z, objname);}

    void setRotateXYZ(double angle, float x, float y, float z, const char *objname = NULL);
    void setRotateXYZ(double angle, const char *objname = NULL);//绕转轴旋转angle度
    float *getRotationAxis(){ return m_RotationAxis; }
    void setRotationAxis(float x, float y, float z, const char *objname = NULL);

    void setAngle(double angle) { m_angle = angle;}
    void setAngle(double angle, const char *objname);
    void updateAngleToMat();


protected:
    Matrix4 mTransformation;

private:
    void setXYZTransition(float xposition, float yposition, float zposition);
    void recureProcess( void *(*pFnProc)(UsrAiNode *), void(*pFnFinaleProc)(void *)); //该函数不用，用作代码模板

    UsrAiNode* mParent; //父结点
    std::list<UsrAiNode *> childrenList; //字结点
    NodeType nodeType;//结点类型

    const char *fileName;//
    const char *mName;//结点名字
    bool ishidden; //true表示隐藏，false显示

    float m_RotationAxis[3];//当前结点的转轴
    double m_angle;//当前结点绕转轴转的角度

    std::list<unsigned int*> meshList;
    std::list<int> showList;//结点下挂的显示列表
};

#endif // USRAINODE_H


#if 0
class usrAiNode
{
public:
    /** Constructor */
    usrAiNode()
    {
        // set all members to zero by default
        mParent = NULL;
        mNumChildren = 0; mChildren = NULL;
        mNumMeshes = 0; mMeshes = NULL;
    }

    /** Construction from a specific name */
    usrAiNode(const std::string& name)
    {
        // set all members to zero by default
        mParent = NULL;
        mNumChildren = 0; mChildren = NULL;
        mNumMeshes = 0; mMeshes = NULL;
        mName = name;
    }

    /** Destructor */
    ~usrAiNode()
    {
        // delete all children recursively
        // to make sure we won't crash if the data is invalid ...
        if (mChildren && mNumChildren)
        {
            for( unsigned int a = 0; a < mNumChildren; a++)
                delete mChildren[a];
        }
        delete [] mChildren;
        delete [] mMeshes;
    }

    /** Searches for a node with a specific name, beginning at this
     *  nodes. Normally you will call this method on the root node
     *  of the scene.
     *
     *  @param name Name to search for
     *  @return NULL or a valid Node if the search was successful.
     */
    inline usrAiNode* FindNode(const aiString& name)
    {
        return FindNode(name.data);
    }

    /** @override
     */
    inline usrAiNode* FindNode(const char* name)
    {
        if (!::strcmp( mName.data,name))return this;
        for (unsigned int i = 0; i < mNumChildren;++i)
        {
            usrAiNode* p = mChildren[i]->FindNode(name);
            if (p)return p;
        }
        // there is definitely no sub node with this name
        return NULL;
    }
    void addNodetoTree(const std::string &objname, usrAiNode *Node);
    void addShowList(int showList);
    void callShowList();
private:
    aiString mName;
    aiMatrix4x4 mTransformation;
    usrAiNode* mParent;
    unsigned int mNumChildren;
    usrAiNode** mChildren;
    unsigned int mNumMeshes;
    unsigned int* mMeshes;
    std::list<int> showList;
};

class usrAiNode : public aiNode
{
public:
    usrAiNode():aiNode(){}
    usrAiNode(const std::string& name):aiNode(name){}
    ~usrAiNode();
    void addNodeToTree(const std::string& inobjname, usrAiNode* Node);
    void addShowListToNode(const std::string& inobjname,int addlist);
    void printShowListsFromNode();
    void delNodeFromTree(const std::string& inobjname, usrAiNode* Node);
    void delNodeReclusive(usrAiNode* Node);
    void delShowList(int showlist);
    void callShowList();
    void setTranslationMatrix(const std::string& inobjname, const aiMatrix4x4& m);

private:

    std::list<int> showList;
};
#endif
