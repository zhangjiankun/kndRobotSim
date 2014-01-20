#include "camera.h"
#include <math.h>
#include "GL/glu.h"

Camera* Camera::m_pCamera = NULL;

/* 构造函数 */
Camera::Camera()
{
    /* 初始化向量值 */
    aiVector3D zero = aiVector3D(0.0, 0.0, 0.0);
    aiVector3D view = aiVector3D(0.0, 1.0, 0.5);
    aiVector3D up   = aiVector3D(0.0, 0.0, 1.0);

    /* 初始化摄像机 */

    //观察位置 Eye
    m_Position	= zero;
    //被观察点
    m_View		= view;
    //倒立还是正立
    m_UpVector	= up;
    //前进速度
    m_Speed     = 0.05f;
    //相机指针
    m_pCamera = this;
}

Camera::~Camera()
{
}

/* 设置摄像机的位置,朝向和向上向量 */
void Camera::setCamera( float positionX, float positionY, float positionZ,
                        float viewX,     float viewY,     float viewZ,
                        float upVectorX, float upVectorY, float upVectorZ)
{
    /* 构造向量 */
    aiVector3D Position	= aiVector3D(positionX, positionY, positionZ);
    aiVector3D View		= aiVector3D(viewX, viewY, viewZ);
    aiVector3D UpVector	= aiVector3D(upVectorX, upVectorY, upVectorZ);

    /* 设置摄像机 */
    m_Position = Position;
    m_View     = View;
    m_UpVector = UpVector;
}

/*  旋转摄像机方向  */
void Camera::rotateView(float angle, float x, float y, float z)
{
    aiVector3D newView;

    /* 计算方向向量 */
    aiVector3D view = m_View - m_Position;

    /* 计算 sin 和cos值 */
    float cosTheta = (float)cos(angle);
    float sinTheta = (float)sin(angle);

    /* 计算旋转向量的x值 */
    newView.x  = (cosTheta + (1 - cosTheta) * x * x)		* view.x;
    newView.x += ((1 - cosTheta) * x * y - z * sinTheta)	* view.y;
    newView.x += ((1 - cosTheta) * x * z + y * sinTheta)	* view.z;

    /* 计算旋转向量的y值 */
    newView.y  = ((1 - cosTheta) * x * y + z * sinTheta)	* view.x;
    newView.y += (cosTheta + (1 - cosTheta) * y * y)		* view.y;
    newView.y += ((1 - cosTheta) * y * z - x * sinTheta)	* view.z;

    /* 计算旋转向量的z值 */
    newView.z  = ((1 - cosTheta) * x * z - y * sinTheta)	* view.x;
    newView.z += ((1 - cosTheta) * y * z + x * sinTheta)	* view.y;
    newView.z += (cosTheta + (1 - cosTheta) * z * z)		* view.z;

    /* 更新摄像机的方向 */
    m_View = m_Position + newView;
}

/* 用鼠标旋转摄像机 */
void Camera::setViewByMouse()
{
#if 0
    /*此函数已放弃。如要使用，在Update处调用即可*/

    /*< 保存当前鼠标位置 */
    POINT mousePos;
    int middleX = GetSystemMetrics(SM_CXSCREEN) >> 1;
    /*< 得到屏幕宽度的一半 */
    int middleY = GetSystemMetrics(SM_CYSCREEN) >> 1;
    /*< 得到屏幕高度的一半 */

    float angleY = 0.0f;/*< 摄像机左右旋转角度 */
    float angleZ = 0.0f;/*< 摄像机上下旋转角度 */
    static float currentRotX = 0.0f;
    /* 得到当前鼠标位置 */
    GetCursorPos(&mousePos);
    ShowCursor(TRUE);
    /* 如果鼠标没有移动,则不用更新 */
    if( (mousePos.x == middleX) && (mousePos.y == middleY) )
        return;     /* 设置鼠标位置在屏幕中心 */
    SetCursorPos(middleX, middleY);
    /* 得到鼠标移动方向 */
    angleY = (float)( (middleX - mousePos.x) ) / 1000.0f;
    angleZ = (float)( (middleY - mousePos.y) ) / 1000.0f;
    static float lastRotX = 0.0f;
    /* 用于保存旋转角度 */
    lastRotX = currentRotX;
    /* 跟踪摄像机上下旋转角度 */
    currentRotX += angleZ;
    /* 如果上下旋转弧度大于1.0,我们截取到1.0并旋转 */
    if(currentRotX > 1.0f)
    {
        currentRotX = 1.0f;

        /* 根据保存的角度旋转方向 */
        if(lastRotX != 1.0f)
        {
            /* 通过叉积找到与旋转方向垂直的向量 */
            Vector3 vAxis = m_View - m_Position;
            vAxis = vAxis.crossProduct(m_UpVector);
            vAxis = vAxis.normalize();

            ///旋转
            rotateView( 1.0f - lastRotX, vAxis.x, vAxis.y, vAxis.z);
        }
    }
    /* 如果旋转弧度小于-1.0,则也截取到-1.0并旋转 */
    else if(currentRotX < -1.0f)
    {
        currentRotX = -1.0f;

        if(lastRotX != -1.0f)
        {

            /* 通过叉积找到与旋转方向垂直的向量 */
            Vector3 vAxis = m_View - m_Position;
            vAxis = vAxis.crossProduct(m_UpVector);
            vAxis = vAxis.normalize();

            ///旋转
            rotateView( -1.0f - lastRotX, vAxis.x, vAxis.y, vAxis.z);
        }
    }
    /* 否则就旋转angleZ度 */
    else
    {
        /* 找到与旋转方向垂直向量 */
        Vector3 vAxis = m_View - m_Position;
        vAxis = vAxis.crossProduct(m_UpVector);
        vAxis = vAxis.normalize();

        ///旋转
        rotateView(angleZ, vAxis.x, vAxis.y, vAxis.z);
    }

    /* 总是左右旋转摄像机 */
    rotateView(angleY, 0, 1, 0);
#endif
}

/* 左右移动摄像机 */
void Camera::yawCamera(float speed)
{
    aiVector3D yaw;
    aiVector3D cross = m_View - m_Position;
    //a × b= [a2b3-a3b2,a3b1-a1b3, a1b2-a2b1]
    yaw.x = cross.y*m_UpVector.z - cross.z*m_UpVector.y;
    yaw.y = cross.z*m_UpVector.x - cross.x*m_UpVector.z;
    yaw.z = cross.x*m_UpVector.y - cross.y*m_UpVector.x;

    ///归一化向量
    yaw = yaw.Normalize();

    m_Position.x += yaw.x * speed;
    m_Position.z += yaw.z * speed;

    m_View.x += yaw.x * speed;
    m_View.z += yaw.z * speed;

}

/* 前后移动摄像机 */
void Camera::moveCamera(float speed)
{
    /* 计算方向向量 */
    aiVector3D vector = m_View - m_Position;
    vector = vector.Normalize();         /*< 单位化 */

    /* 更新摄像机 */
    m_Position.x += vector.x * speed;    /*< 根据速度更新位置 */
    m_Position.y += vector.y * speed;
    m_Position.z += vector.z * speed;

    m_View.x += vector.x * speed;		 /*< 根据速度更新方向 */
    m_View.y += vector.y * speed;
    m_View.z += vector.z * speed;

}

/* 设置视点 */
void Camera::setLook()
{

    /* 设置视口 */
    gluLookAt(m_Position.x, m_Position.y, m_Position.z,
              m_View.x,	 m_View.y,     m_View.z,
              m_UpVector.x, m_UpVector.y, m_UpVector.z);
}
