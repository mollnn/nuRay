#include "glwidget.h"

GlWidget::GlWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{
    vertices = {
        // 位置                  //纹理坐标
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  // 右下
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // 左下
        -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,  // 左下
        0.5f, 0.5f, 0.0f, 1.0f, 1.0f,   // 右上
    };
}

GlWidget::~GlWidget()
{

}

void GlWidget::initializeGL()
{
    this->initializeOpenGLFunctions(); // 初始化opengl函数

    // create and load texture
    pTexture0 = new QOpenGLTexture(QOpenGLTexture::Target2D);
    pTexture0->create();
    pTexture0->setData(QImage("texture.jpg"));

    // create and load shader
    m_shaderTextureShader.addShaderFromSourceFile(QOpenGLShader::Vertex,"./shader.vert");
    m_shaderTextureShader.addShaderFromSourceFile(QOpenGLShader::Fragment,"./shader.frag");
    m_shaderTextureShader.link();

    // create and load vertex data
    QOpenGLVertexArrayObject::Binder{&m_vaoVertexArrayObject};
    m_vboVertexBufferObject.create();
    m_vboVertexBufferObject.bind();
    m_vboVertexBufferObject.allocate(vertices.data(),sizeof(float)*vertices.size());

    // set attribute pointer
    m_shaderTextureShader.setAttributeBuffer("aPos",GL_FLOAT,0,3,sizeof(GLfloat)*5);
    m_shaderTextureShader.enableAttributeArray("aPos");
    m_shaderTextureShader.setAttributeBuffer("aTexCoord",GL_FLOAT,sizeof(GLfloat)*3,2,sizeof(GLfloat)*5);
    m_shaderTextureShader.enableAttributeArray("aTexCoord");
}

void GlWidget::resizeGL(int w, int h)
{
    this->glViewport(0, 0, w, h); //定义视口区域
}

void GlWidget::paintGL()
{
    // clear
    this->glClearColor(0.1,0.5,0.7,1.0);
    this->glClear(GL_COLOR_BUFFER_BIT);

    // bind shader
    m_shaderTextureShader.bind();

    // bind texture and connect to texture unit
    pTexture0->bind(0);
    m_shaderTextureShader.setUniformValue("ourTexture",0);

    // bind vao and draw
    QOpenGLVertexArrayObject::Binder{&m_vaoVertexArrayObject};
    this->glDrawArrays(GL_POLYGON,0,4);
}
