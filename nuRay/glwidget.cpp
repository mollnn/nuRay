#include "glwidget.h"

GlWidget::GlWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{
    vertices_ = {
        0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        -0.5f, 0.5f, 0.0f
    };
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
    if(default_shader_.link() == false)
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
    // clear
    this->glClearColor(0.1, 0.5, 0.7, 1.0);
    this->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // bind shader
    default_shader_.bind();

        
    default_shader_.setUniformValue(default_shader_.uniformLocation("u_mvp"), mvp);

    // bind vao and draw
    QOpenGLVertexArrayObject::Binder{&vao_};
    this->glDrawArrays(GL_TRIANGLES, 0, vertices_.size());
}
