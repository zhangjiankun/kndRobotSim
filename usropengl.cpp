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
    //const GLfloat reflectanceX[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
    //const GLfloat reflectanceY[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
    //const GLfloat reflectanceZ[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
    GLUquadric* quadratic = NULL;

    glNewList(list, GL_COMPILE);
    glPushMatrix();
    transRotaint( xTra,  yTra,  zTra,  xRot,  yRot, zRot);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);


    //glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, reflectanceX);
    //glShadeModel(GL_FLAT);
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
        gluCylinder(quadratic, 0.1f, 0.0f, 0.5f, 16, 16);
    glPopMatrix();

    //glMaterialfv(GL_FRONT, GL_DIFFUSE, reflectanceY);
    //glShadeModel(GL_FLAT);
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
#if 1
    //glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, reflectanceZ);
    //glShadeModel(GL_FLAT);
    glColor3f(0.f, 0.f, 1.0f);//red -> x

    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0, 0.0f, 1.0f);
    glEnd();
    glPushMatrix();
        glTranslatef(0.0, 0.0f, 1.0f);
        //glRotatef(180.0f,0.0f,1.0f,0.0f);
        quadratic = gluNewQuadric();
        gluQuadricNormals(quadratic, GLU_SMOOTH);
        gluQuadricTexture(quadratic, GL_TRUE); // 二次曲面使用纹理
         // 绘制一个圆柱
        gluCylinder(quadratic, 0.2f, 0.0f, 0.5f, 16, 16);
    glPopMatrix();
#endif
    glPopMatrix();
    //glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glEndList();

    return list;
}
