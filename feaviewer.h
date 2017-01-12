#ifndef FEAVIEWER_H
#define FEAVIEWER_H

#include <QGLWidget>
#include "mesh.h"

using namespace std;
// #include <QOpenGLWidget>

class FeaViewer : public QGLWidget
// class FeaViewer : public QOpenGLWidget
{
    Q_OBJECT
public:
    explicit FeaViewer(QWidget *parent = 0);

    bool & meshLoadedState() { return meshLoadedState_; }
    void setMesh(Mesh*mesh);

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);

    QSize minimumSizeHint() const;
    QSize sizeHint() const;
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

signals:
    // signaling rotation from mouse movement
    void xRotationChanged(int angle);
    void yRotationChanged(int angle);
    void zRotationChanged(int angle);


public slots:

    // solts for xyz-rotation slider
    void setXRotation(int angle);
    void setYRotation(int angle);
    void setZRotation(int angle);

private:
    void draw();
    void drawMesh();

    inline const Mesh* mesh() const { return mesh_; }
    inline const double zoomVal() const { return zoomVal_; }

    int xRot;
    int yRot;
    int zRot;
    bool meshLoadedState_;
    Mesh *mesh_;

    QPoint lastPos;
    double zoomVal_;

};

#endif // FEAVIEWER_H
