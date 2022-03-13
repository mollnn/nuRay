#include "../gui/glwidget.h"

GlWidget::GlWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{
    vertices_ = {};
}

GlWidget::~GlWidget()
{
}

void GlWidget::initializeGL()
{
    this->initializeOpenGLFunctions();

    this->glEnable(GL_DEPTH_TEST);
    this->glCullFace(GL_BACK);
    this->glEnable(GL_CULL_FACE);

    // create and load shader
    default_shader_.addShaderFromSourceFile(QOpenGLShader::Vertex, "./shader.vert");
    default_shader_.addShaderFromSourceFile(QOpenGLShader::Fragment, "./shader.frag");
    if (default_shader_.link() == false)
    {
        qDebug() << "link failed";
    }

    // create and load vertex data
    vao_.create();
    vao_.bind();
    vbo_.create();
    vbo_.bind();
    vbo_.allocate(vertices_.data(), sizeof(float) * vertices_.size());
    qDebug() << vertices_.size();

    // set attribute pointer
    default_shader_.setAttributeBuffer("a_pos", GL_FLOAT, 0, 3, sizeof(GLfloat) * 6);
    default_shader_.enableAttributeArray("a_pos");
    default_shader_.setAttributeBuffer("a_normal", GL_FLOAT, sizeof(GLfloat) * 3, 3, sizeof(GLfloat) * 6);
    default_shader_.enableAttributeArray("a_normal");

    vbo_.release();
    vao_.release();
}

void GlWidget::resizeGL(int w, int h)
{
    this->glViewport(0, 0, w, h); //定义视口区域
}

void GlWidget::paintGL()
{
    vao_.bind();
    QMatrix4x4 mvp;
    mvp.perspective(atan(tan(camera_->fov_h * 3.14159 / 180 / 2) * std::max(1.0f, camera_->aspect)) * 180 / 3.14159 * 2, 1, 1, 10000);
    vec3 look_at_center = camera_->pos + camera_->gaze * 100.0f;
    mvp.lookAt({camera_->pos[0], camera_->pos[1], camera_->pos[2]},
               {look_at_center[0], look_at_center[1], look_at_center[2]},
               {camera_->up[0], camera_->up[1], camera_->up[2]});

    // clear
    this->glClearColor(0.1, 0.5, 0.7, 1.0);
    this->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // bind shader
    default_shader_.bind();
    default_shader_.setUniformValue(default_shader_.uniformLocation("u_mvp"), mvp);

    // bind vao and draw
    this->glDrawArrays(GL_TRIANGLES, 0, vertices_.size());
    vao_.release();
}

void GlWidget::mousePressEvent(QMouseEvent *event)
{
    last_mouse_pos_ = event->pos();
}

void GlWidget::mouseReleaseEvent(QMouseEvent *event)
{
    emit cameraChanged();
    this->update();
}

void GlWidget::mouseMoveEvent(QMouseEvent *event)
{
    QPointF mouse_delta = event->pos() - last_mouse_pos_;
    if (event->buttons() & Qt::RightButton)
    {
        camera_->turnH(-mouse_delta.x() * 0.001);
        camera_->turnV(mouse_delta.y() * 0.001);
    }
    if (event->buttons() & Qt::LeftButton)
    {
        camera_->go(-mouse_delta.x() * 1, mouse_delta.y() * 1, 0.0);
    }
    last_mouse_pos_ = event->pos();
    this->update();
}

void GlWidget::wheelEvent(QWheelEvent *event)
{
    camera_->go(0.0, 0.0, event->delta() * 0.1);
    emit cameraChanged();
    this->update();
}

void GlWidget::setVertices(const QVector<float> &vertices)
{
    vertices_ = vertices;
    if (vao_.isCreated())
    {
        qDebug() << "remake";
        vao_.bind();
        vbo_.bind();
        vbo_.allocate(vertices_.data(), sizeof(float) * vertices_.size());
        default_shader_.setAttributeBuffer("a_pos", GL_FLOAT, 0, 3, sizeof(GLfloat) * 6);
        default_shader_.enableAttributeArray("a_pos");
        default_shader_.setAttributeBuffer("a_normal", GL_FLOAT, sizeof(GLfloat) * 3, 3, sizeof(GLfloat) * 6);
        default_shader_.enableAttributeArray("a_normal");
        vbo_.release();
        vao_.release();
    }
    this->update();
}

void GlWidget::setCamera(Camera *camera)
{
    camera_ = camera;
    this->update();
}
