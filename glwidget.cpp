/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui>
#include <QtOpenGL>

#include <math.h>

#include "glwidget.h"
#include "robotmodelcfg.h"
#include <GL/glu.h>

//b zjk
// assimp include files. These three are usually needed.
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "usrainode.h"
#include "loadModel.h"
#include "public.h"
#include "debug.h"
#include "usropengl.h"
#include "Matrices.h"

#include <QTime> // 记时用。

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(parent)
{
    gear1 = 0;
    gear2 = 1;
    gear3 = 0;
    xRot = 0;
    yRot = 0;
    zRot = 0;
    wordx =0;
    wordy =0;
    gear1Rot = 0;
    viewNearLenSteps = 0;
    viewNearLen = getNearLen();
    fullscreen = false;
//    m_modelCfgData = new RobotModelCfg;
    setFocusPolicy(Qt::StrongFocus);
    this->m_robotModelRoot = NULL;
//    QTimer *timer = new QTimer(this);
//    connect(timer, SIGNAL(timeout()), this, SLOT(advanceGears()));
//    timer->start(20);
}

GLWidget::~GLWidget()
{
    makeCurrent();
    glDeleteLists(gear1, 1);
    glDeleteLists(gear2, 1);
    glDeleteLists(gear3, 1);
    glDeleteLists(axis, 1);
    glDeleteTextures(1,texture);
//    delete m_modelCfgData;
    //glDeleteLists(scene_list,1);
}

/*************************************************
Function: // 函数名称
Description: // 函数功能、性能等的描述
 构造如下轴连接关系：
    0           1        2     ...       6        7
    baseAxis __ base
             |
             |_ SAxis __ S
                      |
                      |_ LAxis
                               ...
                                      __ B
                                      |
                                      |_ TAxis __ T

Input: // 输入参数说明，包括每个参数的作
// 用、取值说明及参数间关系。
Output: // 对输出参数的说明。
Return: // 函数返回值的说明
Author: zhangjiankun
Others: // 其它说明
*************************************************/
void GLWidget::creatBasicNodeTree( RobotModelCfg *robotModelCfg)
{
    int i = 0;
    float *positionAxis = NULL;

    if (NULL != m_robotModelRoot)
    {
        delete m_robotModelRoot;
    }

    // 创建baseAxis
    m_robotModelRoot = new UsrAiNode(UsrAiNode::AXIS, robotModelCfg->get_nodeAxisName(0)); //当前结点为坐标轴
    m_robotModelRoot->addShowListToNode(makeWordPlane(), robotModelCfg->get_nodeAxisName(0));

    //生成模型树，加载轴坐标（轴坐标随手臂一起运动，Txyz时针对root坐标的移动如果重合的话，参数为0即可）
    for(i = 1; i < robotModelCfg->getAxisNum(); i++)
    {
        positionAxis = robotModelCfg->get_axisPosition(i-1);
        m_robotModelRoot->addNodeToTree(new UsrAiNode(UsrAiNode::MODULE, robotModelCfg->get_rootName(i-1)),
                                     robotModelCfg->get_nodeAxisName(i-1));
        m_robotModelRoot->addNodeToTree(new UsrAiNode(UsrAiNode::AXIS, robotModelCfg->get_nodeAxisName(i)),
                                     robotModelCfg->get_nodeAxisName(i-1));
        m_robotModelRoot->addShowListToNode(makeWordPlane(), robotModelCfg->get_nodeAxisName(i));

        //移动坐标轴到转轴原点
        m_robotModelRoot->translateXYZ(robotModelCfg->get_nodeAxisName(i-1), positionAxis[0], positionAxis[1], positionAxis[2]);

        //消除转轴移动导致的该坐标下的物体位置移动。
        m_robotModelRoot->translateXYZ(robotModelCfg->get_rootName(i-1), 0-positionAxis[0], 0-positionAxis[1], 0-positionAxis[2]);
        m_robotModelRoot->translateXYZ(robotModelCfg->get_nodeAxisName(i), 0-positionAxis[0], 0-positionAxis[1], 0-positionAxis[2]);
    }

    // 将上面for循环里面的步骤，作用到最后一个结点上。
    m_robotModelRoot->addNodeToTree(new UsrAiNode(UsrAiNode::MODULE,
                                               robotModelCfg->get_rootName(i-1)), robotModelCfg->get_nodeAxisName(i-1));

    positionAxis = robotModelCfg->get_axisPosition(i-1);
    m_robotModelRoot->translateXYZ(robotModelCfg->get_nodeAxisName(i-1), positionAxis[0], positionAxis[1], positionAxis[2]);
    m_robotModelRoot->translateXYZ(robotModelCfg->get_rootName(i-1), 0-positionAxis[0], 0-positionAxis[1], 0-positionAxis[2]);

    //在每个节点上加载手臂模型
    for(int i = 0; i < robotModelCfg->getAxisNum(); i++)
    {
        if( 0 != loadasset(robotModelCfg->get_filename(i)))
        {
            WAR_OUT("load file %s failed!\n", robotModelCfg->get_filename(i));
        }

        float * axisRotation = robotModelCfg->get_axisRotation(i);
        m_robotModelRoot->setFileNameByNode(robotModelCfg->get_filename(i),robotModelCfg->get_rootName(i) );
        m_robotModelRoot->addShowListToNode(makeLoadObj(),robotModelCfg->get_rootName(i));
        m_robotModelRoot->setRotationAxis(axisRotation[0], axisRotation[1], axisRotation[2],
                                       robotModelCfg->get_nodeAxisName(i));
        aiReleaseImport(scene);
    }

}


void GLWidget::setAndUpdateRobotModel(const char *filename)
{

}


void GLWidget::initializeGL()
{
    QTime t;
    t.start();

    //DEBUG_OUT("enter initial GL");
    static const GLfloat lightPos[4] = { 5.0f, 5.0f, 10.0f, 1.0f };
//    static const GLfloat reflectance1[4] = { 0.8f, 0.1f, 0.0f, 1.0f };
    static const GLfloat reflectance2[4] = { 0.0f, 0.8f, 0.2f, 1.0f };
    static const GLfloat reflectance3[4] = { 0.2f, 0.2f, 1.0f, 1.0f };

    loadGLTextures(":/resource/imgs/tiankong.jpg");

    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);


    //显示列表生成
    gear2 = makeGear(reflectance2, 0.5, 1.0, 2.0, 0.7, 10);
    gear3 = makeGear(reflectance3, 1.3, 2.0, 0.5, 0.7, 15);
    axis = makeWordPlane();

    m_robotModelRoot = new UsrAiNode(UsrAiNode::AXIS, "root"); //当前结点为坐标轴
    m_robotModelRoot->addShowListToNode(makeWordPlane());

    glEnable(GL_NORMALIZE);
    glClearColor(0.3f, 0.3f, 0.5f, 0.5f);
    glClearDepth( 1.0 );//Specifies the depth value used when the depth buffer is cleared. The initial value is 1.
    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LEQUAL );

    DEBUG_OUT("initial GL.Time elapsed: %d ms", t.elapsed());


}
inline float GLWidget::getNearLen()
{
    //return 20*(atan(0.1*viewNearLenSteps)+0.5 * 3.1415926);
    return viewNearLenSteps;
}
void GLWidget::paintGL()
{
    aiMatrix4x4 m,n,out;
    int windowWidth = 0;
    int halfHeight = 0;

    QTime t;
    t.start();

    if (fullscreen)
    {
        windowWidth = QApplication::desktop()->width();
        halfHeight = QApplication::desktop()->height();
    }
    else
    {
        windowWidth=width();
        halfHeight=height();
    }
#if 1 // do not del
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-1.0, +1.0, -1.0*halfHeight/windowWidth, 1.0*halfHeight/windowWidth, 1.0*(viewNearLen/10. + 2.), 200.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

#else
    if(windowWidth > halfHeight)
        setViewportSub((windowWidth-halfHeight)/2, halfHeight, halfHeight, halfHeight, 1, 100);
    else
        setViewportSub(0, halfHeight+(halfHeight-windowWidth)/2, windowWidth, windowWidth, 1, 100);
#endif
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    ComputeBackgroundQuad(windowWidth, halfHeight, texture[0]);

    glTranslated(wordx * 0.2 , 0.0, 0.0);
    glTranslated(0.0, wordy * 0.2, 0.0);
    glTranslated(0.0, 0.0, -30.0 + 0*viewNearLen );
    gluLookAt(1.0, 1.0, 1.0,0.0, 0.0, 0.0,0.0, 0.0, 1.0);

    //显示世界坐标中心
    glCallList(axis);
    //glCallList(gear2);

    glRotated(0*xRot / 16., 1.0, 0.0, 0.0);
    glRotated(zRot /16. , 0.0, 1.0, 0.0);
    glRotated(yRot / 16.0 , 0.0, 0.0, 1.0);

#if 1
    glPushMatrix();
#if 0
    glGetDoublev(GL_MODELVIEW_MATRIX,mView);
     set_aiMatrix4x4(m,mView);
    glRotated(xRot / 16.0, 1.0, 0.0, 0.0);
    glRotated(yRot / 16.0, 0.0, 1.0, 0.0);
    glRotated(zRot / 16.0, 0.0, 0.0, 1.0);
    glGetDoublev(GL_MODELVIEW_MATRIX,mViewAfter);
    set_aiMatrix4x4(n,mView);
    out = m.Inverse() * n;
#endif
    if (NULL != m_robotModelRoot)
    {
        m_robotModelRoot->callShowList();
        DEBUG_OUT("call showlist1 GL.Time elapsed: %d ms", t.elapsed());
        m_robotModelRoot->callShowList(UsrAiNode::AXIS);
    }
    glPopMatrix();

#endif
    getfps();
    DEBUG_OUT("paint GL.Time elapsed: %d ms", t.elapsed());
}


void GLWidget::resizeGL(int width, int height)
{
    //int side = qMin(width, height);
    if (fullscreen)
    {
        width = QApplication::desktop()->width();
        height = QApplication::desktop()->height();
    }
    DEBUG_OUT("enter reSize,width %d, height %d",width, height);

    glViewport(0, 0, width, height);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

/*************************************************
Function: // 函数名称
Description: // 函数功能、性能等的描述
    设置objname指定的node的模型变换矩阵。
Input: // 输入参数说明，包括每个参数的作
// 用、取值说明及参数间关系。
Output: // 对输出参数的说明。
Return: // 函数返回值的说明
Author: zhangjiankun
Others: // 其它说明
*************************************************/
void GLWidget::setModelMat(const char *objname, GLfloat Tx, GLfloat Ty, GLfloat Tz, GLfloat Rx, GLfloat Ry, GLfloat Rz)
{
    Matrix4 out;//单位矩阵

#if 1 //使用新的Matrix4数据结构的算法
    //先做移动，当前矩阵左乘以 移动矩阵
    out.translate(Tx, Ty, Tz);
    //再作旋转
    out.rotateX(Rx);
    out.rotateY(Ry);
    out.rotateZ(Rz);
#else //使用 aiMatriX4X4 时的实现算法
    Matrix4 m,
    aiVector3D xyz(Tx,Ty,Tz);
    m.Translation(xyz,m);//生成移动矩阵
    out = out * m; // 右乘移动矩阵
    m.RotationX(Rx*PI/(180),m);
    out = out * m;
    m.RotationY(Ry*PI/(180),m);
    out = out * m;
    m.RotationZ(Rz*PI/(180),m);
    out = out * m;

#endif
    m_robotModelRoot->setTranslationMatrix(out, objname);
}
///////////////////////////////////////////////////////////////////////////////
// configure projection and viewport of sub window
///////////////////////////////////////////////////////////////////////////////
void GLWidget::setViewportSub(int x, int y, int width, int height, float nearPlane, float farPlane)
{
    aiMatrix4x4 m;
    float floatArry[16]={0};

    aiMatrix4x4_to_float(m,floatArry);
    // set viewport
    //glViewport(x, y, width, height);
    //glScissor(x, y, width, height);
    qDebug("%d,%d",x,y);

    // set perspective viewing frustum
    setFrustum(FOV_Y, (float)(width)/height, nearPlane, farPlane, floatArry); // FOV, AspectRatio, NearClip, FarClip

    // copy projection matrix to OpenGL
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(floatArry);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

///////////////////////////////////////////////////////////////////////////////
// set a symmetric perspective frustum with 4 params similar to gluPerspective
// (vertical field of view, aspect ratio, near, far)
///////////////////////////////////////////////////////////////////////////////
void GLWidget::setFrustum(float fovY, float aspectRatio, float front, float back, float floatArry[])
{
    float tangent = tanf(fovY/2 * DEG2RAD);   // tangent of half fovY
    float height = front * tangent;           // half height of near plane
    float width = height * aspectRatio;       // half width of near plane

    // params: left, right, bottom, top, near, far
    setFrustum(-width, width, -height, height, front, back, floatArry);
}

void GLWidget::setFrustum(float l, float r, float b, float t, float n, float f, float floatArry[])
{
    floatArry[0]  =  2 * n / (r - l);
    floatArry[2]  =  (r + l) / (r - l);
    floatArry[5]  =  2 * n / (t - b);
    floatArry[6]  =  (t + b) / (t - b);
    floatArry[10] = -(f + n) / (f - n);
    floatArry[11] = -(2 * f * n) / (f - n);
    floatArry[14] = -1;
    floatArry[15] =  0;
}


void GLWidget::loadGLTextures(const char *fileName)
{
    QImage tex, buf;
    if (!buf.load(fileName))
    {
        //如果载入不成功，自动生成一个32位色的绿色图片。
        qWarning( "Could not read image file, using single-color instead." );
        QImage dummy(128, 128, QImage::Format_RGB32);
        dummy.fill( Qt::green );
        buf = dummy;
    }
    //这里使用了QGLWidget类中提供的一个静态函数converToGLFormat()，专门用来转换图片的，具体情况请参见相应文档。
    tex = QGLWidget::convertToGLFormat( buf );

    //创建一个纹理。告诉OpenGL我们想生成一个纹理名字(如果您想载入多个纹理，加大数字)。
    glGenTextures(1, &texture[0]);

    //使用来自位图数据生成的典型纹理。glBindTexture告诉OpenGL将纹理名字texture[0]绑定到纹理目标上。
    //2D纹理只有高度（在Y轴上）和宽度（在X轴上）。本例中我们告知OpenGL，&texture[0]处的内存已经可用。
    //我们创建的纹理将存储在&texture[0]的指向的内存区域。
    glEnable(GL_TEXTURE_2D);
    glBindTexture( GL_TEXTURE_2D, texture[0] );

    //这里真正的创建纹理。GL_TEXTURE_2D告诉OpenGL此纹理是一个2D纹理。数字零代表图像的详细程度，通常就由它为零去了。
    //数字三是数据的成分数。因为图像是由红色数据，绿色数据，蓝色数据三种组分组成。
    //tex.width()是纹理的宽度。tex.height()是纹理的高度。数字零是边框的值，一般就是零。
    //GL_RGBA 告诉OpenGL图像数据由红、绿、蓝三色数据以及alpha通道数据组成，这个是由于QGLWidget类的converToGLFormat()函数的原因。
    //GL_UNSIGNED_BYTE 意味着组成图像的数据是无符号字节类型的。
    //最后tex.bits()告诉OpenGL纹理数据的来源。
    glTexImage2D( GL_TEXTURE_2D, 0, 3, tex.width(), tex.height(), 0,
      GL_RGBA, GL_UNSIGNED_BYTE, tex.bits() );

    //上面的两行告诉OpenGL在显示图像时，当它比放大得原始的纹理大（GL_TEXTURE_MAG_FILTER）
    //或缩小得比原始得纹理小（GL_TEXTURE_MIN_FILTER）时OpenGL采用的滤波方式。
    //通常这两种情况下我都采用GL_LINEAR。如果您的机器很慢，您也许应该采用GL_NEAREST。
    //过滤的纹理在放大的时候，看起来斑驳的很。您也可以结合这两种滤波方式。在近处时使用GL_LINEAR，远处时GL_NEAREST。
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glDisable(GL_TEXTURE_2D);
}


void GLWidget::getfps(void)
{
    static QString tmpStr = "";
    static int framesPerSecond = 0;//fps的数值
    static int frames = 0;       // 用于存储渲染的帧数
    static bool started = false;
    static QTime time;

    qDebug("frame========================= %d, %d", frames, time.elapsed() );

    if (time.elapsed() < 1000 && started == true)//如果已经开始计时，则每秒刷新一次
    {
        frames ++;
        renderText(width() - 60, height() - 20, "FPS: " + tmpStr, QFont("times", 15));//最终结果在窗口中渲染
    }
    else
    {
        framesPerSecond = frames;

        tmpStr.setNum(framesPerSecond);
        frames= 0;
        started = true;
        time.start();
    }
}


void GLWidget::keyPressEvent( QKeyEvent *e )
{
    DEBUG_OUT("keyPressed!");
    switch ( e->key() )
    {
    case Qt::Key_Up:
        xRot -= 0.2;
        updateGL();
        break;
    case Qt::Key_Down:
        xRot += 0.2;
        updateGL();
        break;
    case Qt::Key_Left:
        yRot -= 0.2;
        updateGL();
        break;
    case Qt::Key_Right:
        yRot += 0.2;
        updateGL();
        break;
    case Qt::Key_Escape:
    case Qt::Key_F2:
        fullscreen = !fullscreen;
        DEBUG_OUT("%d",fullscreen);
        if ( fullscreen )
        {
            DEBUG_OUT("full screen!");
            setWindowFlags(Qt::Window);
            showFullScreen();
        }
        else
        {
            setWindowFlags(Qt::SubWindow);
            showNormal();
            resizeGL(width(),height());
            DEBUG_OUT("SubWindow! %d",width());
        }

        //update();

        break;

        //close();
    }
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}
void GLWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    xRot = 0;
    yRot = 0;
    zRot = 0;
    wordx =0;
    wordy =0;
    gear1Rot = 0;
    viewNearLenSteps = 0;
    viewNearLen = getNearLen();
    updateGL();
    event->accept();
}
void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - lastPos.x();
    int dy = event->y() - lastPos.y();
    //if (event->isAccepted())         return;

    if (event->buttons() & Qt::LeftButton) {
        setXRotation(xRot + 8 * dy);
        setYRotation(yRot + 8 * dx);
    } else if (event->buttons() & Qt::RightButton) {
        setXRotation(xRot + 8 * dy);
        setZRotation(zRot + 8 * dx);
    } else if (event->buttons() & Qt::MidButton) {
        wordx += 1.0*dx;
        wordy += -1.0*dy;
        DEBUG_OUT("wordx %f, wordy %f",wordx,wordy);
        updateGL();
    }
    lastPos = event->pos();
    event->accept();
}


void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->isAccepted())
        return;

    if (event->button() == Qt::LeftButton) {
        event->accept();
    }

    if (event->button() == Qt::RightButton) {
        event->accept();
    }

    if (event->button() == Qt::MidButton) {
        event->accept();
    }
}

void GLWidget::wheelEvent(QWheelEvent *event)
{
    int numDegrees = event->delta() / 8;
    int numSteps = numDegrees / 15;
    viewNearLenSteps += numSteps;
    //atan(0.1*viewNearLenSteps)+0.5 * 3.1415926 is 0-3
    viewNearLen = getNearLen();//这个函数应该把全局变量作为入参
    DEBUG_OUT("event->orientation() %d,numSteps =%d,viewNearLen=%f  ",event->orientation(),numSteps,viewNearLen);
    if (event->orientation() == Qt::Horizontal) {
        //scrollHorizontally(numSteps);
    } else {
       // scrollVertically(numSteps);
    }
    updateGL();
    event->accept();
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
void GLWidget::setXTransition(double xposition, const char * currentNodeName)
{
    UsrAiNode * theFoundedNod = NULL;

    if (NULL != m_robotModelRoot) {
        theFoundedNod = m_robotModelRoot->FindNode(currentNodeName);
        if (NULL == theFoundedNod) {
            DEBUG_OUT("%s,%d:node not exist",__FILE__,__LINE__);
            return;
        }
        theFoundedNod->setXTransition(xposition);
        updateGL();
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
void GLWidget::setYTransition(double yposition, const char * currentNodeName)
{
    UsrAiNode * theFoundedNod = NULL;

    if (NULL != m_robotModelRoot) {
        theFoundedNod = m_robotModelRoot->FindNode(currentNodeName);
        if (NULL == theFoundedNod) {
            DEBUG_OUT("%s,%d:node not exist",__FILE__,__LINE__);
            return;
        }

        theFoundedNod->setYTransition(yposition);
        updateGL();
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
void GLWidget::setZTransition(double zposition, const char * currentNodeName)
{
    UsrAiNode * theFoundedNod = NULL;

    if (NULL != m_robotModelRoot) {
        theFoundedNod = m_robotModelRoot->FindNode(currentNodeName);
        if (NULL == theFoundedNod) {
            DEBUG_OUT("%s,%d:node not exist",__FILE__,__LINE__);
            return;
        }
        theFoundedNod->setZTransition(zposition);
        updateGL();
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
void GLWidget::setXRotation(int angle)
{
    normalizeAngle(&angle);
    if (angle != xRot) {
        xRot = angle;
        emit xRotationChanged(angle);
        updateGL();
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
void GLWidget::setYRotation(int angle)
{
    normalizeAngle(&angle);
    if (angle != yRot) {
        yRot = angle;
        emit yRotationChanged(angle);
        updateGL();
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
void GLWidget::setZRotation(int angle)
{
    normalizeAngle(&angle);
    if (angle != zRot) {
        zRot = angle;
        emit zRotationChanged(angle);
        updateGL();
    }
}

void GLWidget::advanceGears()
{
    gear1Rot += 2 * 16;
    updateGL();
}

GLuint GLWidget::makeLoadObj()
{
    GLuint list = glGenLists(1);
    //const GLfloat reflectance[4] = { 0.3f, 0.3f, 0.3f, 1.0f };

    glNewList(list, GL_COMPILE);
    //glMaterialfv(GL_FRONT_FACE, GL_AMBIENT_AND_DIFFUSE, reflectance); 材质应当放到模型中去显示。
    //glShadeModel(GL_FLAT);

    recursive_render(scene, scene->mRootNode);
    glEndList();

    return list;
}


///////////////////////////////////////////////////////////////////////////////
// draw a grid on the xz plane
///////////////////////////////////////////////////////////////////////////////
unsigned int GLWidget::drawGrid(float size, float step)
{
    GLuint list = glGenLists(1);
    glNewList(list, GL_COMPILE);
    // disable lighting
    glDisable(GL_LIGHTING);

    glBegin(GL_LINES);

    glColor3f(0.3f, 0.3f, 0.3f);
    for(float i=step; i <= size; i+= step)
    {
        glVertex3f(-size, 0,  i);   // lines parallel to X-axis
        glVertex3f( size, 0,  i);
        glVertex3f(-size, 0, -i);   // lines parallel to X-axis
        glVertex3f( size, 0, -i);

        glVertex3f( i, 0, -size);   // lines parallel to Z-axis
        glVertex3f( i, 0,  size);
        glVertex3f(-i, 0, -size);   // lines parallel to Z-axis
        glVertex3f(-i, 0,  size);
    }

    // x-axis
    glColor3f(0.5f, 0, 0);
    glVertex3f(-size, 0, 0);
    glVertex3f( size, 0, 0);

    // z-axis
    glColor3f(0,0,0.5f);
    glVertex3f(0, 0, -size);
    glVertex3f(0, 0,  size);

    glEnd();

    // enable lighting back
    glEnable(GL_LIGHTING);
    glEndList();

    return list;
}
GLuint GLWidget::makeGear(const GLfloat *reflectance, GLdouble innerRadius,
                          GLdouble outerRadius, GLdouble thickness,
                          GLdouble toothSize, GLint toothCount)
{
    const double Pi = 3.14159265358979323846;

    GLuint list = glGenLists(1);
    glNewList(list, GL_COMPILE);
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, reflectance);

    GLdouble r0 = innerRadius;
    GLdouble r1 = outerRadius - toothSize / 2.0;
    GLdouble r2 = outerRadius + toothSize / 2.0;
    GLdouble delta = (2.0 * Pi / toothCount) / 4.0;
    GLdouble z = thickness / 2.0;
    int i, j;

    glShadeModel(GL_FLAT);

    for (i = 0; i < 2; ++i) {
        GLdouble sign = (i == 0) ? +1.0 : -1.0;

        glNormal3d(0.0, 0.0, sign);

        glBegin(GL_QUAD_STRIP);
        for (j = 0; j <= toothCount; ++j) {
            GLdouble angle = 2.0 * Pi * j / toothCount;
        glVertex3d(r0 * cos(angle), r0 * sin(angle), sign * z);
        glVertex3d(r1 * cos(angle), r1 * sin(angle), sign * z);
        glVertex3d(r0 * cos(angle), r0 * sin(angle), sign * z);
        glVertex3d(r1 * cos(angle + 3 * delta), r1 * sin(angle + 3 * delta),
                       sign * z);
        }
        glEnd();

        glBegin(GL_QUADS);
        for (j = 0; j < toothCount; ++j) {
            GLdouble angle = 2.0 * Pi * j / toothCount;
        glVertex3d(r1 * cos(angle), r1 * sin(angle), sign * z);
        glVertex3d(r2 * cos(angle + delta), r2 * sin(angle + delta),
                       sign * z);
        glVertex3d(r2 * cos(angle + 2 * delta), r2 * sin(angle + 2 * delta),
                       sign * z);
        glVertex3d(r1 * cos(angle + 3 * delta), r1 * sin(angle + 3 * delta),
                       sign * z);
        }
        glEnd();
    }

    glBegin(GL_QUAD_STRIP);
    for (i = 0; i < toothCount; ++i) {
        for (j = 0; j < 2; ++j) {
            GLdouble angle = 2.0 * Pi * (i + (j / 2.0)) / toothCount;
            GLdouble s1 = r1;
            GLdouble s2 = r2;
            if (j == 1)
                qSwap(s1, s2);

        glNormal3d(cos(angle), sin(angle), 0.0);
        glVertex3d(s1 * cos(angle), s1 * sin(angle), +z);
        glVertex3d(s1 * cos(angle), s1 * sin(angle), -z);

        glNormal3d(s2 * sin(angle + delta) - s1 * sin(angle),
                       s1 * cos(angle) - s2 * cos(angle + delta), 0.0);
        glVertex3d(s2 * cos(angle + delta), s2 * sin(angle + delta), +z);
        glVertex3d(s2 * cos(angle + delta), s2 * sin(angle + delta), -z);
        }
    }
    glVertex3d(r1, 0.0, +z);
    glVertex3d(r1, 0.0, -z);
    glEnd();

    glShadeModel(GL_SMOOTH);

    glBegin(GL_QUAD_STRIP);
    for (i = 0; i <= toothCount; ++i) {
    GLdouble angle = i * 2.0 * Pi / toothCount;
    glNormal3d(-cos(angle), -sin(angle), 0.0);
    glVertex3d(r0 * cos(angle), r0 * sin(angle), +z);
    glVertex3d(r0 * cos(angle), r0 * sin(angle), -z);
    }
    glEnd();

    glEndList();

    return list;
}

void GLWidget::drawGear(GLuint gear, GLdouble dx, GLdouble dy, GLdouble dz,
                        GLdouble angle)
{
    glPushMatrix();
    glTranslated(dx, dy, dz);
    glRotated(angle, 0.0, 0.0, 1.0);
    glCallList(gear);
    glPopMatrix();
}

void GLWidget::normalizeAngle(int *angle)
{
    while (*angle < 0)
        *angle += 360 * 16;
    while (*angle > 360 * 16)
        *angle -= 360 * 16;
}
