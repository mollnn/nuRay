#include "widget.h"
#include <QDoubleValidator>
#include <QSizePolicy>

Widget::Widget(QWidget *parent)
    : QWidget(parent),
      img_render_result_(QSize(1, 1), QImage::Format_RGB888),
      btn_render_("Render")
{

    label_render_result_.setMinimumSize(QSize(512, 384));
    QSizePolicy qsp = label_render_result_.sizePolicy();
    qsp.setHorizontalPolicy(QSizePolicy::MinimumExpanding);
    qsp.setVerticalStretch(5);
    qsp.setHorizontalStretch(5);
    label_render_result_.setSizePolicy(qsp);

    grid_layout_.addWidget(&label_render_result_, 0, 0, 50, 50);
    grid_layout_.addWidget(&btn_render_, 49, 54, 1, 1);

    connect(&btn_render_, &QPushButton::clicked, [&]()
            { renderRT(); });

    glwidget_preview_.setFixedHeight(512);
    glwidget_preview_.setFixedWidth(512);

    grid_layout_.addWidget(&glwidget_preview_, 0, 50, 10, 10);

    camera_.pos = {50.0f, 320.0f, 500.0f};
    camera_.gaze = vec3(-0.2f, -0.5f, -1.0f).normalized();
    vec3 camera_hand = vec3(1.0f, 0.0f, 0.1f).normalized();
    camera_.up = camera_hand.cross(camera_.gaze).normalized();
    camera_.fov_h = 20.0f;
    camera_.aspect = 1.0;

    text_edit_scene_.setText("test/test2.obj -p 0 1000 0 -s 50 \nmitsuba/mitsuba.obj -p 0 0 0 -s 50");
    scene_loader_.fromSceneDescription("test/test2.obj -p 0 1000 0 -s 50 \nmitsuba/mitsuba.obj -p 0 0 0 -s 50");

    updateVertices();

    glwidget_preview_.camera_ = &camera_;

    auto &triangles = scene_loader_.getTriangles();
    this->renderer_.prepare(triangles);

    last_review_render_time_ = QTime::currentTime().addSecs(-1);

    connect(&glwidget_preview_, &GlWidget::cameraChanged, [&]()
            { 
        line_edit_cam_pos_x_.setText(QString::number(camera_.pos[0], 'f', 4));
        line_edit_cam_pos_y_.setText(QString::number(camera_.pos[1], 'f', 4));
        line_edit_cam_pos_z_.setText(QString::number(camera_.pos[2], 'f', 4));
        line_edit_yaw_.setText(QString::number(camera_.toEuler()[0], 'f', 4));
        line_edit_pitch_.setText(QString::number(camera_.toEuler()[1], 'f', 4));
        line_edit_roll_.setText(QString::number(camera_.toEuler()[2], 'f', 4));
        renderRT_preview(); });

    bindLineEdit(line_edit_fov_h_, camera_.fov_h);
    grid_layout_.addWidget(&line_edit_fov_h_, 10, 51, 1, 4);
    grid_layout_.addWidget(&label_fov_h_, 10, 50, 1, 1);

    bindLineEdit(line_edit_aspect_, camera_.aspect);
    grid_layout_.addWidget(&line_edit_aspect_, 11, 51, 1, 4);
    grid_layout_.addWidget(&label_aspect_, 11, 50, 1, 1);

    bindLineEdit(line_edit_img_w_, img_width_);
    grid_layout_.addWidget(&line_edit_img_w_, 12, 51, 1, 4);
    grid_layout_.addWidget(&label_img_w_, 12, 50, 1, 1);

    bindLineEdit(line_edit_img_h_, img_height_);
    grid_layout_.addWidget(&line_edit_img_h_, 13, 51, 1, 4);
    grid_layout_.addWidget(&label_img_h_, 13, 50, 1, 1);

    bindLineEdit(line_edit_preview_level_, preview_level_);
    grid_layout_.addWidget(&line_edit_preview_level_, 14, 51, 1, 4);
    grid_layout_.addWidget(&label_preview_level_, 14, 50, 1, 1);

    bindLineEdit(line_edit_spp_, spp_);
    grid_layout_.addWidget(&line_edit_spp_, 15, 51, 1, 4);
    grid_layout_.addWidget(&label_spp_, 15, 50, 1, 1);

    bindLineEdit(line_edit_spp_preview_, spp_preview_);
    grid_layout_.addWidget(&line_edit_spp_preview_, 16, 51, 1, 4);
    grid_layout_.addWidget(&label_spp_preview_, 16, 50, 1, 1);

    bindLineEdit(line_edit_cam_pos_x_, camera_.pos[0]);
    grid_layout_.addWidget(&line_edit_cam_pos_x_, 20, 51, 1, 4);
    grid_layout_.addWidget(&label_cam_pos_x_, 20, 50, 1, 1);

    bindLineEdit(line_edit_cam_pos_y_, camera_.pos[1]);
    grid_layout_.addWidget(&line_edit_cam_pos_y_, 21, 51, 1, 4);
    grid_layout_.addWidget(&label_cam_pos_y_, 21, 50, 1, 1);

    bindLineEdit(line_edit_cam_pos_z_, camera_.pos[2]);
    grid_layout_.addWidget(&line_edit_cam_pos_z_, 22, 51, 1, 4);
    grid_layout_.addWidget(&label_cam_pos_z_, 22, 50, 1, 1);

    grid_layout_.addWidget(&line_edit_yaw_, 30, 51, 1, 4);
    grid_layout_.addWidget(&label_yaw_, 30, 50, 1, 1);
    line_edit_yaw_.setValidator(new QDoubleValidator(-1e9, 1e9, 4, this));
    line_edit_yaw_.setText(QString::number(camera_.toEuler()[0], 'f', 4));
    connect(&line_edit_yaw_, &QLineEdit::editingFinished, [&]()
            { if(line_edit_yaw_.text().toFloat() == camera_.toEuler()[0]) return; camera_.fromEuler(line_edit_yaw_.text().toFloat(), line_edit_pitch_.text().toFloat(), line_edit_roll_.text().toFloat()); renderRT_preview(); });

    grid_layout_.addWidget(&line_edit_pitch_, 31, 51, 1, 4);
    grid_layout_.addWidget(&label_pitch_, 31, 50, 1, 1);
    line_edit_pitch_.setValidator(new QDoubleValidator(-1e9, 1e9, 4, this));
    line_edit_pitch_.setText(QString::number(camera_.toEuler()[1], 'f', 4));
    connect(&line_edit_pitch_, &QLineEdit::editingFinished, [&]()
            { if(line_edit_pitch_.text().toFloat() == camera_.toEuler()[1]) return; camera_.fromEuler(line_edit_yaw_.text().toFloat(), line_edit_pitch_.text().toFloat(), line_edit_roll_.text().toFloat()); renderRT_preview(); });

    grid_layout_.addWidget(&line_edit_roll_, 32, 51, 1, 4);
    grid_layout_.addWidget(&label_roll_, 32, 50, 1, 1);
    line_edit_roll_.setValidator(new QDoubleValidator(-1e9, 1e9, 4, this));
    line_edit_roll_.setText(QString::number(camera_.toEuler()[2], 'f', 4));
    connect(&line_edit_roll_, &QLineEdit::editingFinished, [&]()
            { if(line_edit_roll_.text().toFloat() == camera_.toEuler()[2]) return; camera_.fromEuler(line_edit_yaw_.text().toFloat(), line_edit_pitch_.text().toFloat(), line_edit_roll_.text().toFloat()); renderRT_preview(); });

    btn_load_scene_.setText("Load");
    grid_layout_.addWidget(&label_scene_, 40, 50, 1, 1);
    grid_layout_.addWidget(&text_edit_scene_, 41, 50, 8, 5);
    grid_layout_.addWidget(&btn_load_scene_, 49, 50, 1, 1);

    connect(&btn_load_scene_, &QPushButton::clicked, [&]()
            {
        scene_loader_.fromSceneDescription(text_edit_scene_.toPlainText().toStdString());
        renderer_.prepare(scene_loader_.getTriangles());
        updateVertices();
        renderRT_preview(); 
    });

    label_cam_pos_x_.setText(("Cam Pos X"));
    label_cam_pos_y_.setText(("Cam Pos Y"));
    label_cam_pos_z_.setText(("Cam Pos Z"));
    label_spp_.setText(("SPP"));
    label_spp_preview_.setText(("Preview SPP"));
    label_yaw_.setText(("Cam Yaw"));
    label_pitch_.setText(("Cam Pitch"));
    label_roll_.setText(("Cam Roll"));
    label_fov_h_.setText(("Cam Fov(V)"));
    label_aspect_.setText(("Cam Aspect"));
    label_img_w_.setText(("Img W"));
    label_img_h_.setText(("Img H"));
    label_preview_level_.setText(("Preview Level"));
    label_scene_.setText("Scene Description");

    this->setLayout(&grid_layout_);
    this->update();
}

Widget::~Widget()
{
}

void Widget::renderRT()
{
    // Render
    QTime timer;
    timer.start();
    std::cout << "Loading scene..." << std::endl;

    auto &triangles = scene_loader_.getTriangles();
    std::cout << "Loading scene ok, " << timer.elapsed() * 0.001 << " secs used" << std::endl;

    int final_width = std::min(label_render_result_.width(), label_render_result_.height() * img_width_ / img_height_);
    int final_height = std::min(label_render_result_.height(), label_render_result_.width() * img_height_ / img_width_);

    int padding_width = label_render_result_.width() - final_width;
    int padding_height = label_render_result_.height() - final_height;

    this->renderer_.render(camera_, triangles, img_render_result_, spp_, img_width_, img_height_);
    label_render_result_.setPixmap(QPixmap::fromImage(img_render_result_.scaled(QSize(final_width, final_height)).copy(-padding_width / 2, -padding_height / 2, label_render_result_.width(), label_render_result_.height())));
}

void Widget::renderRT_preview()
{
    if (last_review_render_time_.msecsTo(QTime::currentTime()) < 500)
        return;

    last_review_render_time_ = QTime::currentTime();

    auto &triangles = scene_loader_.getTriangles();

    int final_width = std::min(label_render_result_.width(), label_render_result_.height() * img_width_ / img_height_);
    int final_height = std::min(label_render_result_.height(), label_render_result_.width() * img_height_ / img_width_);
    int padding_width = label_render_result_.width() - final_width;
    int padding_height = label_render_result_.height() - final_height;

    this->renderer_.render(camera_, triangles, img_render_result_, spp_preview_, img_width_ / preview_level_, img_height_ / preview_level_);
    label_render_result_.setPixmap(QPixmap::fromImage(img_render_result_.scaled(QSize(final_width, final_height)).copy(-padding_width / 2, -padding_height / 2, label_render_result_.width(), label_render_result_.height())));
}

void Widget::updateVertices()
{
    glwidget_preview_.setVertices(QVector<float>::fromStdVector(scene_loader_.getVerticesNormals()));
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
