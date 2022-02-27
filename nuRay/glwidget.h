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

private:
    QOpenGLShaderProgram default_shader_;
    QOpenGLBuffer vbo_;
    QOpenGLVertexArrayObject vao_;

    QVector<float> vertices_;
    QMatrix4x4 mvp;
    Camera *camera_ = nullptr;
    QPointF last_mouse_pos_;

public:
    GlWidget(QWidget *parent = 0);
    ~GlWidget();

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

    void setVertices(const QVector<float> &vertices);
    void setCamera(Camera *camera);

protected:
    virtual void initializeGL() override;
    virtual void resizeGL(int w, int h) override;
    virtual void paintGL() override;

signals:
    void cameraChanged();
};

#endif // WIDGET_H
