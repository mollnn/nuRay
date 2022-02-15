#include "glwidget.h"

GlWidget::GlWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{
    vertices_ = {
        0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        -0.5f, 0.5f, 0.0f};
}

GlWidget::~GlWidget()
{
}

void GlWidget::initializeGL()
{
    this->initializeOpenGLFunctions(); // 初始化opengl函数

    this->glEnable(GL_DEPTH_TEST);

    // create and load shader
    default_shader_.addShaderFromSourceFile(QOpenGLShader::Vertex, "./shader.vert");
    default_shader_.addShaderFromSourceFile(QOpenGLShader::Fragment, "./shader.frag");
    if (default_shader_.link() == false)
    {
        qDebug() << "link failed";
    }

    // create and load vertex data
    QOpenGLVertexArrayObject::Binder{&vao_};
    vbo_.create();
    vbo_.bind();
    vbo_.allocate(vertices_.data(), sizeof(float) * vertices_.size());
    qDebug() << vertices_.size();

    // set attribute pointer
    default_shader_.setAttributeBuffer("a_pos", GL_FLOAT, 0, 3, sizeof(GLfloat) * 6);
    default_shader_.enableAttributeArray("a_pos");
    default_shader_.setAttributeBuffer("a_normal", GL_FLOAT, sizeof(GLfloat) * 3, 3, sizeof(GLfloat) * 6);
    default_shader_.enableAttributeArray("a_normal");
}

void GlWidget::resizeGL(int w, int h)
{
    this->glViewport(0, 0, w, h); //定义视口区域
}

void GlWidget::paintGL()
{
    QMatrix4x4 mvp;
    mvp.perspective(camera->fov_h * 180.0f / 3.14159f * 2.0, camera->aspect, 0.1, 5000);
    vec3 look_at_center = camera->pos + camera->gaze * 100.0f;
    mvp.lookAt({camera->pos[0], camera->pos[1], camera->pos[2]},
               {look_at_center[0], look_at_center[1], look_at_center[2]},
               {camera->up[0], camera->up[1], camera->up[2]});

    // clear
    this->glClearColor(0.1, 0.5, 0.7, 1.0);
    this->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // bind shader
    default_shader_.bind();
    default_shader_.setUniformValue(default_shader_.uniformLocation("u_mvp"), mvp);

    // bind vao and draw
    QOpenGLVertexArrayObject::Binder{&vao_};
    this->glDrawArrays(GL_TRIANGLES, 0, vertices_.size());

    this->update();
}

void GlWidget::mousePressEvent(QMouseEvent *event)
{
    last_mouse_pos = event->pos();
}

void GlWidget::mouseReleaseEvent(QMouseEvent *event)
{
    emit renderPreview();
}

void GlWidget::mouseMoveEvent(QMouseEvent *event)
{
    QPointF mouse_delta = event->pos() - last_mouse_pos;
    if (event->buttons() & Qt::RightButton)
    {
        camera->turnH(-mouse_delta.x() * 0.001);
        camera->turnV(mouse_delta.y() * 0.001);
    }
    if (event->buttons() & Qt::LeftButton)
    {
        camera->go(-mouse_delta.x() * 1, mouse_delta.y() * 1, 0.0);
    }
    last_mouse_pos = event->pos();
}

void GlWidget::wheelEvent(QWheelEvent *event)
{
    camera->go(0.0, 0.0, event->delta() * 0.1);
    emit renderPreview();
}