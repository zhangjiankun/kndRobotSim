#ifndef CAMERA_H
#define CAMERA_H
#include"assimp/scene.h"
/* 摄像机类 */
class Camera
{
public:

    Camera();
    ~Camera();

    /* 获得摄像机状态方法 */
    aiVector3D getPosition()   {	return m_Position;		}
    aiVector3D getView()	    {	return m_View;			}
    aiVector3D getUpVector()   {	return m_UpVector;		}
    float   getSpeed()  {   return m_Speed;         }

    /* 设置速度 */
    void setSpeed(float speed)
    {
        m_Speed  = speed;
    }

    /* 设置摄像机的位置, 观察点和向上向量 */
    void setCamera(float positionX, float positionY, float positionZ,
                   float viewX,     float viewY,     float viewZ,
                   float upVectorX, float upVectorY, float upVectorZ);

    /* 旋转摄像机方向 */
    void rotateView(float angle, float X, float Y, float Z);

    /* 根据鼠标设置摄像机观察方向 */
    void setViewByMouse();

    /* 左右摄像机移动 */
    void yawCamera(float speed);

    /* 前后移动摄像机 */
    void moveCamera(float speed);

    /* 放置摄像机 */
    void setLook();

    /* 得到摄像机指针 */
    static Camera* GetCamera(void) { return m_pCamera;}

private:
        /* 摄像机属性 */
    static Camera  *m_pCamera;      /* 当前全局摄像机指针 */
    aiVector3D     m_Position;      /* 位置 */
    aiVector3D     m_View;          /* 朝向 */
    aiVector3D     m_UpVector;      /* 向上向量 */
    float          m_Speed;         /* 速度 */

};

#endif // CAMERA_H
