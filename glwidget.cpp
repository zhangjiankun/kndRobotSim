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

//{xyz位移,xyz旋转角度}即在什么位置绕xyz分别转多少度
float axes[][6] = {{0.,-0.,-1.,-90,0,0.},
                   {0.,0.,-1.},
                   {0.,0.,-1.},
                   {0.,0.,-1.},
                   {0.,0.,-1.},
                   {0.,0.,-1.}};
const char *rootName[] = {"world","S","L","U","R","B","T"};//world理解为 arm.
const char *nodeAxis[] = {"worldAxis","SAxis","LAxis","UAxis","RAxis","BAxis","TAxis"};
const char *filename[] = {
    "/home/zjk/work/RobotSim/grabber/resource/robotModels/FANUC_M710iC_50-0.stl",
    "/home/zjk/work/RobotSim/grabber/resource/robotModels/FANUC_M710iC_50-1.stl",
    "/home/zjk/work/RobotSim/grabber/resource/robotModels/FANUC_M710iC_50-2.stl",
    "/home/zjk/work/RobotSim/grabber/resource/robotModels/FANUC_M710iC_50-3.stl",
    "/home/zjk/work/RobotSim/grabber/resource/robotModels/FANUC_M710iC_50-4.stl",
    "/home/zjk/work/RobotSim/grabber/resource/robotModels/FANUC_M710iC_50-5.stl",
    "/home/zjk/work/RobotSim/grabber/resource/robotModels/FANUC_M710iC_50-6.stl",
};

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


    setFocusPolicy(Qt::StrongFocus);
    //QTimer *timer = new QTimer(this);
    //connect(timer, SIGNAL(timeout()), this, SLOT(advanceGears()));
    //timer->start(20);
}

GLWidget::~GLWidget()
{
    makeCurrent();
    glDeleteLists(gear1, 1);
    glDeleteLists(gear2, 1);
    glDeleteLists(gear3, 1);
    //glDeleteLists(scene_list,1);

}


void GLWidget::creatBasicNodeTree()
{
    usrAiNodeRoot = new usrAiNode(rootName[0]);
    usrAiNode *tmp0 = new usrAiNode(nodeAxis[0]);
    usrAiNodeRoot->addNodeToTree(rootName[0],tmp0 );

    //生成模型树，加载轴坐标（轴坐标随手臂一起运动，Txyz时针对root坐标的移动如果重合的话，参数为0即可）
    for(int i = 1; i < sizeof(rootName)/sizeof(char *); i++)
    {
        usrAiNode *tmp = new usrAiNode(rootName[i]);
        usrAiNode *tmp2 = new usrAiNode(nodeAxis[i]);
        usrAiNodeRoot->addNodeToTree(rootName[i-1],tmp);
        usrAiNodeRoot->addNodeToTree(nodeAxis[i-1],tmp2);
        usrAiNodeRoot->addShowListToNode(nodeAxis[i-1],makeWordPlane());
    }

    //在每个节点上加载手臂模型
    for(int i = 0; i < sizeof(filename)/sizeof(char *) && i < sizeof(rootName)/sizeof(char *); i++)
    {
        //setModelMat(rootName[i], 0., 0., 0., axes[i][3],axes[i][4],axes[i][5]);

        if( 0 != loadasset(filename[i])) {
            DEBUG_OUT("load file %s failed!\n",filename[i]);
        }
        DEBUG_OUT("load file %s success",filename[i]);
        usrAiNodeRoot->addNodeFileToNode(rootName[i],filename[i]);
        usrAiNodeRoot->addShowListToNode(rootName[i],makeLoadObj());
        aiReleaseImport(scene);
    }


}

void GLWidget::initializeGL()
{
    DEBUG_OUT("enter initial GL");
    static const GLfloat lightPos[4] = { 5.0f, 5.0f, 10.0f, 1.0f };
    static const GLfloat reflectance1[4] = { 0.8f, 0.1f, 0.0f, 1.0f };
    static const GLfloat reflectance2[4] = { 0.0f, 0.8f, 0.2f, 1.0f };
    static const GLfloat reflectance3[4] = { 0.2f, 0.2f, 1.0f, 1.0f };

    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);

    creatBasicNodeTree();

    gear2 = makeGear(reflectance2, 0.5, 1.0, 2.0, 0.7, 10);
    gear3 = makeGear(reflectance3, 1.3, 2.0, 0.5, 0.7, 15);

    DEBUG_OUT("scene = %d",scene);

    glEnable(GL_NORMALIZE);
    glClearColor(0.2f, 0.2f, 0.2f, 0.5f);

}
inline float GLWidget::getNearLen()
{
    //return 20*(atan(0.1*viewNearLenSteps)+0.5 * 3.1415926);
    return viewNearLenSteps;
}
void GLWidget::paintGL()
{
    aiMatrix4x4 m,n,out;

    int windowWidth=width();
    int halfHeight=height();
    DEBUG_OUT("enter paintGL. fullscreen = %d", fullscreen);
    if (fullscreen)
    {
        windowWidth = QApplication::desktop()->width();
        halfHeight = QApplication::desktop()->height();
    }
#if 1 // do not del
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-1.0, +1.0, -1.0*halfHeight*2/windowWidth, 1.0*halfHeight*2/windowWidth +0.5, viewNearLen/10 + 2, 200.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
#else
    if(windowWidth > halfHeight)
        setViewportSub((windowWidth-halfHeight)/2, halfHeight, halfHeight, halfHeight, 1, 100);
    else
        setViewportSub(0, halfHeight+(halfHeight-windowWidth)/2, windowWidth, windowWidth, 1, 100);
#endif

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glTranslated(wordx * 0.2 , 0.0, 0.0);
    glTranslated(0.0, wordy * 0.2-10, 0.0);
    glTranslated(0.0, 0.0, -30.0 + viewNearLen );
    glCallList(drawGrid(8, 1));

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
    //设置子坐标系，即旋转轴。
    axes[1][5] = xRot/16.;
    axes[2][4] = yRot/16.;
    for(int i = 0; i < sizeof(filename)/sizeof(char *) && i < sizeof(rootName)/sizeof(char *); i++)
    {
        setModelMat(rootName[i], axes[i][0], axes[i][1], axes[i][2], axes[i][3],axes[i][4],axes[i][5]);
    }
    usrAiNodeRoot->callShowList();
    usrAiNodeRoot->printAllNode();
    //usrAiNodeRoot->printShowListsFromRoot();
    //glCallList(gear1);
    //drawGear(gear1, -3.0, -2.0, 0.0, gear1Rot / 16.0);
    //drawGear(gear2, +3.1, -2.0, 0.0, -2.0 * (gear1Rot / 16.0) - 9.0);
    //glRotated(+90.0, 1.0, 0.0, 0.0);
    //drawGear(gear3, -3.1, -1.8, -2.2, +2.0 * (gear1Rot / 16.0) - 2.0);

    glPopMatrix();

#endif

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
}

void GLWidget::setModelMat(const char *objname, GLfloat Tx, GLfloat Ty, GLfloat Tz, GLfloat Rx, GLfloat Ry, GLfloat Rz)
{
    aiMatrix4x4 m,out;
    aiVector3D xyz(Tx,Ty,Tz);
#if 1
    m.Translation(xyz,m);
    out = out * m;
    m.RotationX(Rx*PI/(180),m);
    out = out * m;
    m.RotationY(Ry*PI/(180),m);
    out = out * m;
    m.RotationZ(Rz*PI/(180),m);
    out = out * m;

#else
    aiMatrix4x4 n;
    double mView[16] = {0}, mViewAfter[16]={0};
    glGetDoublev(GL_MODELVIEW_MATRIX,mView);
     set_aiMatrix4x4(m,mView);
    glRotated(90, 1.0, 0.0, 0.0);
    glRotated(yRot / 16.0, 0.0, 1.0, 0.0);
    glRotated(zRot / 16.0, 0.0, 0.0, 1.0);
    qDebug("xRot %f %f %f",xRot,yRot,zRot);
#endif
    usrAiNodeRoot->setTranslationMatrix(objname,out);
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

void GLWidget::setXRotation(int angle)
{
    normalizeAngle(&angle);
    if (angle != xRot) {
        xRot = angle;
        emit xRotationChanged(angle);
        updateGL();
    }
}

void GLWidget::setYRotation(int angle)
{
    normalizeAngle(&angle);
    if (angle != yRot) {
        yRot = angle;
        emit yRotationChanged(angle);
        updateGL();
    }
}

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
