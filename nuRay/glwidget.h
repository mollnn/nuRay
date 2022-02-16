#ifndef GL_WIDGET_H
#define GL_WIDGET_H

#include <QImage>
#include <QOpenGLWidget>
#include <QOpenGLContext>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLExtraFunctions>
#include <QOpenGLTexture>
#include <QMouseEvent>

#include "camera.h"

class GlWidget : public QOpenGLWidget, public QOpenGLExtraFunctions
{
    Q_OBJECT

    // private:
public:
    QOpenGLShaderProgram default_shader_;
    QOpenGLBuffer vbo_;
    QOpenGLVertexArrayObject vao_;

public:
    QVector<float> vertices_;
    QMatrix4x4 mvp;

    GlWidget(QWidget *parent = 0);
    ~GlWidget();

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
void wheelEvent(QWheelEvent *event);


    Camera* camera = nullptr;

    QPointF last_mouse_pos;
    

protected:
    virtual void initializeGL() override;
    virtual void resizeGL(int w, int h) override;
    virtual void paintGL() override;

signals:
    void glwChanged();
};

#endif // WIDGET_H
