#include "feaviewer.h"
#include <QtOpenGL>
#include <QtWidgets>

FeaViewer::FeaViewer(QWidget *parent) :
    QGLWidget(parent)
//     QOpenGLWidget(parent)
//   QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    xRot = 0;
    yRot = 0;
    zRot = 0;
}

void FeaViewer::initializeGL()
{
    qglClearColor(Qt::black);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    static GLfloat lightPosition[4] = {0, 0, 10, 1.0};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
}

void FeaViewer::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -10.0);
    glRotatef(xRot/16.0, 1.0, 0.0, 0.0);
    glRotatef(yRot/16.0, 0.0, 1.0, 0.0);
    glRotatef(zRot/16.0, 0.0, 0.0, 1.0);
    draw();
}

void FeaViewer::resizeGL(int width, int height)
{
    int side = qMin(width, height);
    glViewport((width - side)/2, (height-side)/2, side, side);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

#ifndef QT_OPENGL_ES_1
    glOrtho(-2, +2, -2, +2, 1.0, 15.0); // glOrthf(...)
#else
    glOrtho(-2, +2, -2, +2, 1.0, 15.0);
#endif
    glMatrixMode(GL_MODELVIEW);
}

QSize FeaViewer::minimumSizeHint() const
{
    return QSize(50,50);
}

QSize FeaViewer::sizeHint() const
{
    return QSize(400, 400);
}

static void qNormalizeAngle(int &angle)
{
    while (angle < 0)
        angle += 360 * 16;
    while (angle > 360)
        angle -= 360 * 16;
}

void FeaViewer::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}

void FeaViewer::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - lastPos.x();
    int dy = event->y() - lastPos.y();

    if (event->buttons() & Qt::LeftButton) {
       setXRotation(xRot + 8 * dy);
       setYRotation(yRot + 8 * dx);
    } else if (event->buttons() & Qt::RightButton) {
        setXRotation(xRot + 8 * dy);
        setYRotation(yRot + 8 * dx);
    }

    lastPos = event->pos();
}

void FeaViewer::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if(angle != xRot) {
        xRot = angle;
        emit xRotationChanged(angle);
        updateGL();
    }
}

void FeaViewer::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if(angle != yRot) {
        yRot = angle;
        emit yRotationChanged(angle);
        updateGL();
    }
}

void FeaViewer::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if(angle != zRot) {
        zRot = angle;
        emit zRotationChanged(angle);
        updateGL();
    }
}

void FeaViewer::draw()
{
    qglColor(Qt::red);
    glBegin(GL_QUADS);
        glNormal3f(0, 0, -1);
        glVertex3f(-1, -1, 0);
        glVertex3f(-1, 1, 0);
        glVertex3f(1, 1, 0);
        glVertex3f(1, -1, 0);

    glEnd();

    glBegin(GL_TRIANGLES);
        glNormal3f(0,-1,0.707);
        glVertex3f(-1,-1,0);
        glVertex3f(1,-1,0);
        glVertex3f(0,0,1.2);
    glEnd();
    glBegin(GL_TRIANGLES);
        glNormal3f(1,0, 0.707);
        glVertex3f(1,-1,0);
        glVertex3f(1,1,0);
        glVertex3f(0,0,1.2);
    glEnd();
    glBegin(GL_TRIANGLES);
        glNormal3f(0,1,0.707);
        glVertex3f(1,1,0);
        glVertex3f(-1,1,0);
        glVertex3f(0,0,1.2);
    glEnd();
    glBegin(GL_TRIANGLES);
        glNormal3f(-1,0,0.707);
        glVertex3f(-1,1,0);
        glVertex3f(-1,-1,0);
        glVertex3f(0,0,1.2);
    glEnd();
}
