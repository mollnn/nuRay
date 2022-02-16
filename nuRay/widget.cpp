#include "widget.h"
#include "ui_widget.h"
#include <QDoubleValidator>
#include <QSizePolicy>

Widget::Widget(QWidget *parent)
    : QWidget(parent), ui(new Ui::Widget),
      render_result(QSize(1, 1), QImage::Format_RGB888),
      btn_render("Render")
{

    l.setMinimumSize(QSize(512, 384));
    QSizePolicy qsp = l.sizePolicy();
    qsp.setHorizontalPolicy(QSizePolicy::MinimumExpanding);
    qsp.setVerticalStretch(5);
    qsp.setHorizontalStretch(5);
    l.setSizePolicy(qsp);

    grid.addWidget(&l, 0, 0, 50, 50);
    grid.addWidget(&btn_render, 49, 54, 1, 1);

    connect(&btn_render, &QPushButton::clicked, [&]()
            { renderRT(); });

    glw.setFixedHeight(512);
    glw.setFixedWidth(512);

    grid.addWidget(&glw, 0, 50, 10, 10);

    camera.pos = {50.0f, 320.0f, 500.0f};
    camera.gaze = vec3(-0.2f, -0.5f, -1.0f).normalized();
    vec3 camera_hand = vec3(1.0f, 0.0f, 0.1f).normalized();
    camera.up = camera_hand.cross(camera.gaze).normalized();
    camera.fov_h = 20.0f;
    camera.aspect = 1.0;

    custom_materials.push_back(new MatLight(vec3(1.0, 1.0, 1.0) * 10));
    custom_materials.push_back(new MatLambert(vec3(0.2, 0.1, 0.1)));
    custom_materials.push_back(new MatLambert(vec3(0.7, 0.7, 0.7)));

    loader.loadObj("test/test2.obj", {0.0f, 1000.0f, 0.0f}, 20.0f, custom_materials[0]);
    loader.loadObj("mitsuba/mitsuba.obj", {0.0f, 0.0f, 0.0f}, 50.0f);
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

    bindLineEdit(line_edit_fov_h, camera.fov_h);
    grid.addWidget(&line_edit_fov_h, 10, 51, 1, 4);
    grid.addWidget(&label_fov_h, 10, 50, 1, 1);

    bindLineEdit(line_edit_aspect, camera.aspect);
    grid.addWidget(&line_edit_aspect, 11, 51, 1, 4);
    grid.addWidget(&label_aspect, 11, 50, 1, 1);

    bindLineEdit(line_edit_img_w, img_width);
    grid.addWidget(&line_edit_img_w, 12, 51, 1, 4);
    grid.addWidget(&label_img_w, 12, 50, 1, 1);

    bindLineEdit(line_edit_img_h, img_height);
    grid.addWidget(&line_edit_img_h, 13, 51, 1, 4);
    grid.addWidget(&label_img_h, 13, 50, 1, 1);

    bindLineEdit(line_edit_preview_level, preview_level);
    grid.addWidget(&line_edit_preview_level, 14, 51, 1, 4);
    grid.addWidget(&label_preview_level, 14, 50, 1, 1);

    bindLineEdit(line_edit_spp, spp);
    grid.addWidget(&line_edit_spp, 15, 51, 1, 4);
    grid.addWidget(&label_spp, 15, 50, 1, 1);

    bindLineEdit(line_edit_spp_preview, spp_preview);
    grid.addWidget(&line_edit_spp_preview, 16, 51, 1, 4);
    grid.addWidget(&label_spp_preview, 16, 50, 1, 1);

    bindLineEdit(line_edit_cam_pos_x, camera.pos[0]);
    grid.addWidget(&line_edit_cam_pos_x, 20, 51, 1, 4);
    grid.addWidget(&label_cam_pos_x, 20, 50, 1, 1);

    bindLineEdit(line_edit_cam_pos_y, camera.pos[1]);
    grid.addWidget(&line_edit_cam_pos_y, 21, 51, 1, 4);
    grid.addWidget(&label_cam_pos_y, 21, 50, 1, 1);

    bindLineEdit(line_edit_cam_pos_z, camera.pos[2]);
    grid.addWidget(&line_edit_cam_pos_z, 22, 51, 1, 4);
    grid.addWidget(&label_cam_pos_z, 22, 50, 1, 1);

    grid.addWidget(&line_edit_yaw, 30, 51, 1, 4);
    grid.addWidget(&label_yaw, 30, 50, 1, 1);
    line_edit_yaw.setValidator(new QDoubleValidator(-1e9, 1e9, 4, this));
    line_edit_yaw.setText(QString::number(camera.toEuler()[0], 'f', 4));
    connect(&line_edit_yaw, &QLineEdit::editingFinished, [&]()
            { if(line_edit_yaw.text().toFloat() == camera.toEuler()[0]) return; camera.fromEuler(line_edit_yaw.text().toFloat(), line_edit_pitch.text().toFloat(), line_edit_roll.text().toFloat()); renderRT_preview(); });

    grid.addWidget(&line_edit_pitch, 31, 51, 1, 4);
    grid.addWidget(&label_pitch, 31, 50, 1, 1);
    line_edit_pitch.setValidator(new QDoubleValidator(-1e9, 1e9, 4, this));
    line_edit_pitch.setText(QString::number(camera.toEuler()[1], 'f', 4));
    connect(&line_edit_pitch, &QLineEdit::editingFinished, [&]()
            { if(line_edit_pitch.text().toFloat() == camera.toEuler()[1]) return; camera.fromEuler(line_edit_yaw.text().toFloat(), line_edit_pitch.text().toFloat(), line_edit_roll.text().toFloat()); renderRT_preview(); });

    grid.addWidget(&line_edit_roll, 32, 51, 1, 4);
    grid.addWidget(&label_roll, 32, 50, 1, 1);
    line_edit_roll.setValidator(new QDoubleValidator(-1e9, 1e9, 4, this));
    line_edit_roll.setText(QString::number(camera.toEuler()[2], 'f', 4));
    connect(&line_edit_roll, &QLineEdit::editingFinished, [&]()
            { if(line_edit_roll.text().toFloat() == camera.toEuler()[2]) return; camera.fromEuler(line_edit_yaw.text().toFloat(), line_edit_pitch.text().toFloat(), line_edit_roll.text().toFloat()); renderRT_preview(); });

    label_cam_pos_x.setText(("Cam Pos X"));
    label_cam_pos_y.setText(("Cam Pos Y"));
    label_cam_pos_z.setText(("Cam Pos Z"));
    label_spp.setText(("SPP"));
    label_spp_preview.setText(("Preview SPP"));
    label_yaw.setText(("Cam Yaw"));
    label_pitch.setText(("Cam Pitch"));
    label_roll.setText(("Cam Roll"));
    label_fov_h.setText(("Cam Fov(V)"));
    label_aspect.setText(("Cam Aspect"));
    label_img_w.setText(("Img W"));
    label_img_h.setText(("Img H"));
    label_preview_level.setText(("Preview Level"));

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
    // Render
    QTime timer;
    timer.start();
    std::cout << "Loading scene..." << std::endl;

    auto &triangles = loader.getTriangles();
    std::cout << "Loading scene ok, " << timer.elapsed() * 0.001 << " secs used" << std::endl;

    int final_width = std::min(l.width(), l.height() * img_width / img_height);
    int final_height = std::min(l.height(), l.width() * img_height / img_width);

    int padding_width = l.width() - final_width;
    int padding_height = l.height() - final_height;

    this->renderer.render(camera, triangles, render_result, spp, final_width, final_height);
    l.setPixmap(QPixmap::fromImage(render_result.scaled(QSize(final_width, final_height)).copy(-padding_width / 2, -padding_height / 2, l.width(), l.height())));
}

void Widget::renderRT_preview()
{
    if (last_update.msecsTo(QTime::currentTime()) < 500)
        return;

    last_update = QTime::currentTime();

    auto &triangles = loader.getTriangles();

    int final_width = std::min(l.width(), l.height() * img_width / img_height);
    int final_height = std::min(l.height(), l.width() * img_height / img_width);
    int padding_width = l.width() - final_width;
    int padding_height = l.height() - final_height;

    this->renderer.render(camera, triangles, render_result, spp_preview, img_width / preview_level, img_height / preview_level);
    l.setPixmap(QPixmap::fromImage(render_result.scaled(QSize(final_width, final_height)).copy(-padding_width / 2, -padding_height / 2, l.width(), l.height())));
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
