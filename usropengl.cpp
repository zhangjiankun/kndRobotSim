#include"usropengl.h"

void transRotaint(GLfloat xTra, GLfloat yTra, GLfloat zTra, GLfloat xRot, GLfloat yRot, GLfloat zRot)
{
    glRotatef(xRot,1,0,0);
    glRotatef(yRot,0,1,0);
    glRotatef(zRot,0,0,1);
    glTranslatef(xTra,0,0);
    glTranslatef(0,yTra,0);
    glTranslatef(0,0,zTra);
}

GLuint makeWordPlane(GLfloat xTra, GLfloat yTra, GLfloat zTra, GLfloat xRot, GLfloat yRot, GLfloat zRot)
{

    GLuint list = glGenLists(1);
    GLUquadric* quadratic = NULL;

    glNewList(list, GL_COMPILE);
    glPushMatrix();
    //在绘制每个物体前应该把当前状态先保存到堆栈中，再等绘制完后取出原来的状态
    glPushAttrib(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_CURRENT_BIT);
    transRotaint( xTra,  yTra,  zTra,  xRot,  yRot, zRot);
    glDisable(GL_LIGHTING);
    //glDisable(GL_DEPTH_TEST);


    glColor3f(1.f, 0.0f, 0.0f);//red -> x
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(1.0, 0.0f, 0.0f);
    glEnd();
    glPushMatrix();
        glTranslatef(1.0, 0.0f, 0.0f);
        glRotatef(90.0f,0.0f,1.0f,0.0f);
        quadratic = gluNewQuadric();
        gluQuadricNormals(quadratic, GLU_SMOOTH);
        gluQuadricTexture(quadratic, GL_TRUE); // 二次曲面使用纹理
         // 绘制一个圆柱
        gluCylinder(quadratic, 0.2f, 0.0f, 0.5f, 16, 16);
    glPopMatrix();

    glColor3f(0.f, 1.0f, 0.0f);//red -> x

    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0, 1.0f, 0.0f);
    glEnd();
    glPushMatrix();
        glTranslatef(0.0, 1.0f, 0.0f);
        glRotatef(90.0f,-1.0f,0.0f,0.0f);
        quadratic = gluNewQuadric();
        gluQuadricNormals(quadratic, GLU_SMOOTH);
        gluQuadricTexture(quadratic, GL_TRUE); // 二次曲面使用纹理
         // 绘制一个圆柱
        gluCylinder(quadratic, 0.2f, 0.0f, 0.5f, 16, 16);
    glPopMatrix();

    glColor3f(0.f, 0.f, 1.0f);//red -> x

    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0, 0.0f, 1.0f);
    glEnd();
    glPushMatrix();
        glTranslatef(0.0, 0.0f, 1.0f);
        quadratic = gluNewQuadric();
        gluQuadricNormals(quadratic, GLU_SMOOTH);
        gluQuadricTexture(quadratic, GL_TRUE); // 二次曲面使用纹理
         // 绘制一个圆柱
        gluCylinder(quadratic, 0.2f, 0.0f, 0.5f, 16, 16);
    glPopMatrix();

    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glPopAttrib();
    glPopMatrix();
    glEndList();

    return list;
}

/*
背景图片和3d场景都在透视模式下显示，此时为了精确实现图片满屏，需要将窗口四个角的坐标投影到3d场景中，
计算出对应的四边形四个角坐标，然后图片贴上去。步骤如下：
1. 窗口四角 投影到3d场景中，使用gluUnProject函数 计算出新的quad
2. 关闭深度测试
3. 绘制quad，贴图
4. 开启深度测试
*/
void ComputeBackgroundQuad(GLint width, GLint height, GLuint textureID)
{
    GLint   viewport[4] = {0};
    GLdouble projMatrix[16] = {0};
    GLdouble modelMatrix[16] = {0};
    struct vertex
    {
        //GLdouble tu, tv;
        GLdouble x,y,z;
    }bgVertices[4] = {0.0};

    const double winZ = 0.1;

    glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
    glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
    glGetIntegerv(GL_VIEWPORT, viewport);

    // 窗口四角 投影到3d场景中，使用gluUnProject函数 计算出新的quad,逆时针
    gluUnProject(0, 0, winZ, modelMatrix, projMatrix, viewport,
                 &bgVertices[0].x, &bgVertices[0].y, &bgVertices[0].z);

    gluUnProject(width, 0, winZ, modelMatrix, projMatrix, viewport,
                 &bgVertices[1].x, &bgVertices[1].y, &bgVertices[1].z);

    gluUnProject(width, height, winZ, modelMatrix, projMatrix, viewport,
                 &bgVertices[2].x, &bgVertices[2].y, &bgVertices[2].z);

    gluUnProject(0, height, winZ, modelMatrix, projMatrix, viewport,
                 &bgVertices[3].x, &bgVertices[3].y, &bgVertices[3].z);
    //qDebug("----->w:%d h:%d texture:%d", width, height, textureID);
    //qDebug("----->%d %d", viewport[2], viewport[3]);和width height是一样的
    //for(unsigned int i = 0; i < 4; i++)   {
    //    qDebug("----->%f %f %f", bgVertices[i].x,bgVertices[i].y,bgVertices[i].z);
    //}

    //2. 关闭深度测试
    glDisable(GL_DEPTH_TEST);

    //3. 绘制quad，贴图
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glBindTexture( GL_TEXTURE_2D, textureID );

    glShadeModel( GL_SMOOTH );
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

#if 1
        glBegin( GL_QUADS );
        glTexCoord2f( 0.0, 0.0 ); glVertex3f(bgVertices[0].x, bgVertices[0].y, bgVertices[0].z);
        glTexCoord2f( 1.0, 0.0 ); glVertex3f(bgVertices[1].x, bgVertices[1].y, bgVertices[1].z);
        glTexCoord2f( 1.0, 1.0 ); glVertex3f(bgVertices[2].x, bgVertices[2].y, bgVertices[2].z);
        glTexCoord2f( 0.0, 1.0 ); glVertex3f(bgVertices[3].x, bgVertices[3].y, bgVertices[3].z);
        glEnd();
#else   //顶点阵列方式不可用
        bgVertices[0].tu = 0.0; bgVertices[0].tv = 0.0;
        bgVertices[1].tu = 1.0; bgVertices[1].tv = 0.0;
        bgVertices[2].tu = 1.0; bgVertices[2].tv = 1.0;
        bgVertices[3].tu = 0.0; bgVertices[3].tv = 1.0;
        glEnableClientState( GL_VERTEX_ARRAY );
        glEnableClientState( GL_TEXTURE_COORD_ARRAY );
        glInterleavedArrays( GL_T2F_V3F, 0, bgVertices );
        glDrawArrays( GL_QUADS, 0, 4 );
        glDisableClientState( GL_VERTEX_ARRAY );
        glDisableClientState( GL_TEXTURE_COORD_ARRAY );
#endif

    glDisable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);

    //4. 开启深度测试
    glEnable(GL_DEPTH_TEST);

}

