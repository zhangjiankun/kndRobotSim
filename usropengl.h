#ifndef USROPENGL_H
#define USROPENGL_H
#include <QtOpenGL>
#include <GL/glu.h>
void transRotaint(GLfloat xTra, GLfloat yTra, GLfloat zTra, GLfloat xRot, GLfloat yRot, GLfloat zRot);
GLuint makeWordPlane(GLfloat xTra=0, GLfloat yTra=0, GLfloat zTra=0, GLfloat xRot=0., GLfloat yRot=0., GLfloat zRot=0.);
#endif // USROPENGL_H
