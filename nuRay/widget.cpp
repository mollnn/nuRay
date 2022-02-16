#include "widget.h"
#include "ui_widget.h"
#include <QDoubleValidator>

Widget::Widget(QWidget *parent)
    : QWidget(parent), ui(new Ui::Widget),
      render_result(QSize(1, 1), QImage::Format_RGB888),
      btn_render("Render")
{

    l.setFixedSize(QSize(1280, 1280));
    grid.addWidget(&l, 0, 10, 10, 10);
    grid.addWidget(&btn_render, 10, 10, 1, 1);

    connect(&btn_render, &QPushButton::clicked, [&]()
            { renderRT(); });

    l.setPixmap(QPixmap::fromImage(render_result.scaled(QSize(1280, 1280))));

    glw.setFixedHeight(500);
    glw.setFixedWidth(500);

    grid.addWidget(&glw, 0, 0, 10, 10);

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
    // loader.loadObj("sponza/sponza.obj", {0.0f, 0.0f, 0.0f}, 1.0f);

    updateVertices();

    glw.camera = &camera;

    auto &triangles = loader.getTriangles();
    this->renderer.prepare(triangles);

    last_update = QTime::currentTime().addSecs(-1);

    connect(&glw, &GlWidget::glwChanged, [&]()
            { 
        line_edit_cam_pos_x.setText(QString::number(camera.pos[0], 'f', 4));
        line_edit_cam_pos_y.setText(QString::number(camera.pos[1], 'f', 4));
        line_edit_cam_pos_z.setText(QString::number(camera.pos[2], 'f', 4));
        line_edit_yaw.setText(QString::number(camera.toEuler()[0], 'f', 4));
        line_edit_pitch.setText(QString::number(camera.toEuler()[1], 'f', 4));
        line_edit_roll.setText(QString::number(camera.toEuler()[2], 'f', 4));

        renderRT_preview(); });

    bindLineEdit(line_edit_spp, spp);
    grid.addWidget(&line_edit_spp, 0, 20, 1, 1);

    bindLineEdit(line_edit_spp_preview, spp_preview);
    grid.addWidget(&line_edit_spp_preview, 1, 20, 1, 1);

    bindLineEdit(line_edit_cam_pos_x, camera.pos[0]);
    grid.addWidget(&line_edit_cam_pos_x, 3, 20, 1, 1);

    bindLineEdit(line_edit_cam_pos_y, camera.pos[1]);
    grid.addWidget(&line_edit_cam_pos_y, 4, 20, 1, 1);

    bindLineEdit(line_edit_cam_pos_z, camera.pos[2]);
    grid.addWidget(&line_edit_cam_pos_z, 5, 20, 1, 1);

    grid.addWidget(&line_edit_yaw, 7, 20, 1, 1);
    line_edit_yaw.setValidator(new QDoubleValidator(-1e9, 1e9, 4, this));
    line_edit_yaw.setText(QString::number(camera.toEuler()[0], 'f', 4));
    connect(&line_edit_yaw, &QLineEdit::editingFinished, [&]()
            { if(line_edit_yaw.text().toFloat() == camera.toEuler()[0]) return; camera.fromEuler(line_edit_yaw.text().toFloat(), line_edit_pitch.text().toFloat(), line_edit_roll.text().toFloat()); renderRT_preview(); });

    grid.addWidget(&line_edit_pitch, 8, 20, 1, 1);
    line_edit_pitch.setValidator(new QDoubleValidator(-1e9, 1e9, 4, this));
    line_edit_pitch.setText(QString::number(camera.toEuler()[1], 'f', 4));
    connect(&line_edit_pitch, &QLineEdit::editingFinished, [&]()
            { if(line_edit_pitch.text().toFloat() == camera.toEuler()[1]) return; camera.fromEuler(line_edit_yaw.text().toFloat(), line_edit_pitch.text().toFloat(), line_edit_roll.text().toFloat()); renderRT_preview(); });

    grid.addWidget(&line_edit_roll, 9, 20, 1, 1);
    line_edit_roll.setValidator(new QDoubleValidator(-1e9, 1e9, 4, this));
    line_edit_roll.setText(QString::number(camera.toEuler()[2], 'f', 4));
    connect(&line_edit_roll, &QLineEdit::editingFinished, [&]()
            { if(line_edit_roll.text().toFloat() == camera.toEuler()[2]) return; camera.fromEuler(line_edit_yaw.text().toFloat(), line_edit_pitch.text().toFloat(), line_edit_roll.text().toFloat()); renderRT_preview(); });

    this->setLayout(&grid);
    this->update();
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

    auto &triangles = loader.getTriangles();
    std::cout << "Loading scene ok, " << timer.elapsed() * 0.001 << " secs used" << std::endl;

    this->renderer.render(camera, triangles, render_result, spp);
    l.setPixmap(QPixmap::fromImage(render_result.scaled(QSize(1280, 1280))));
}

void Widget::renderRT_preview()
{
    if (last_update.msecsTo(QTime::currentTime()) < 500)
        return;

    last_update = QTime::currentTime();

    const int RSIZE = 32;
    render_result = QImage(QSize(RSIZE, RSIZE), QImage::Format_RGB888);
    camera.img_width = RSIZE;
    camera.img_height = RSIZE;

    auto &triangles = loader.getTriangles();
    this->renderer.render(camera, triangles, render_result, spp_preview);
    l.setPixmap(QPixmap::fromImage(render_result.scaled(QSize(1280, 1280))));

    last_update = QTime::currentTime();
}

void Widget::updateVertices()
{
    glw.vertices_ = QVector<float>::fromStdVector(loader.getVertices());
}

void Widget::bindLineEdit(QLineEdit &line_edit, float &var)
{
    line_edit.setValidator(new QDoubleValidator(-1e9, 1e9, 4, this));
    line_edit.setText(QString::number(var, 'f', 4));
    connect(&line_edit, &QLineEdit::editingFinished, [&]()
            { var = line_edit.text().toDouble(); renderRT_preview(); });
}

void Widget::bindLineEdit(QLineEdit &line_edit, int &var)
{
    line_edit.setValidator(new QIntValidator(-1e9, 1e9, this));
    line_edit.setText(QString::number(var));
    connect(&line_edit, &QLineEdit::editingFinished, [&]()
            { var = line_edit.text().toDouble(); renderRT_preview(); });
}