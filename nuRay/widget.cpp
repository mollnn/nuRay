#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent), ui(new Ui::Widget),
      render_result(QSize(1, 1), QImage::Format_RGB888),
      btn_render("Render")
{

    l.setFixedSize(QSize(1280, 1280));
    grid.addWidget(&l, 0, 1, 1, 1);
    grid.addWidget(&btn_render, 1, 1, 1, 1);

    connect(&btn_render, &QPushButton::clicked, [&]()
            { renderRT(); });

    l.setPixmap(QPixmap::fromImage(render_result.scaled(QSize(1280, 1280))));

    glw.setFixedHeight(500);
    glw.setFixedWidth(500);

    grid.addWidget(&glw, 0, 0, 1, 1);

    this->setLayout(&grid);
    this->update();

    camera.pos = {100.0f, 320.0f, 500.0f};
    camera.gaze = vec3(-0.2f, -0.5f, -1.0f).normalized();
    vec3 camera_hand = vec3(1.0f, 0.0f, 0.1f).normalized();
    camera.up = camera_hand.cross(camera.gaze).normalized();
    camera.fov_h = 20.0f * 3.14159f / 180.0f;
    camera.aspect = 1.0;

    custom_materials.push_back(new MatLight(vec3(1.0, 1.0, 1.0) * 10));
    custom_materials.push_back(new MatLambert(vec3(0.2, 0.1, 0.1)));
    custom_materials.push_back(new MatLambert(vec3(0.7, 0.7, 0.7)));

    loader.loadObj("test/test2.obj", {0.0f, 1000.0f, 0.0f}, 20.0f, custom_materials[0]);
    loader.loadObj("mitsuba/mitsuba.obj", {0.0f, 0.0f, 0.0f}, 100.0f);

    updateVertices();

    glw.camera = &camera;

    auto& triangles = loader.getTriangles();
    this->renderer.prepare(triangles);

    connect(&glw, &GlWidget::renderPreview, [&]()
            { renderRT_preview(); });
}

Widget::~Widget()
{
    for (auto i : custom_materials)
        delete i;
}

void Widget::renderRT()
{
    const int RSIZE = 128;
    render_result = QImage(QSize(RSIZE, RSIZE), QImage::Format_RGB888);
    camera.img_width = RSIZE;
    camera.img_height = RSIZE;

    // Render
    QTime timer;
    timer.start();
    std::cout << "Loading scene..." << std::endl;

    auto& triangles = loader.getTriangles();
    std::cout << "Loading scene ok, " << timer.elapsed() * 0.001 << " secs used" << std::endl;

    this->renderer.render(camera, triangles, render_result);
    l.setPixmap(QPixmap::fromImage(render_result.scaled(QSize(1280, 1280))));
}

void Widget::renderRT_preview()
{
    const int RSIZE = 32;
    render_result = QImage(QSize(RSIZE, RSIZE), QImage::Format_RGB888);
    camera.img_width = RSIZE;
    camera.img_height = RSIZE;

    auto& triangles = loader.getTriangles();
    this->renderer.render(camera, triangles, render_result);
    l.setPixmap(QPixmap::fromImage(render_result.scaled(QSize(1280, 1280))));
}

void Widget::updateVertices()
{
    glw.vertices_ = QVector<float>::fromStdVector(loader.getVertices());
}
