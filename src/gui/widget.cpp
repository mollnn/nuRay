#include "../gui/widget.h"
#include <QDoubleValidator>
#include <QSizePolicy>
#include <QApplication>
#include "../renderer/rendererpt.h"
#include "../renderer/rendererptls.h"
#include "../renderer/rendererpssmlt.h"
#include "../renderer/rendererbdpt.h"
#include "../renderer/rendererpm.h"
#include "../renderer/renderernrc.h"
#include "../scene/envmap.h"
#include "../utils/utils.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent),
      framebuffer_(QSize(1, 1), QImage::Format_RGB888),
      btn_apply_("Apply"),
      btn_render_("Render"),
      btn_cancel_("Cancel")
{

    label_render_result_.setMinimumSize(QSize(512, 384));
    QSizePolicy qt_size_policy = label_render_result_.sizePolicy();
    qt_size_policy.setHorizontalPolicy(QSizePolicy::MinimumExpanding);
    qt_size_policy.setVerticalStretch(5);
    qt_size_policy.setHorizontalStretch(5);
    label_render_result_.setSizePolicy(qt_size_policy);

    grid_layout_.addWidget(&label_render_result_, 0, 0, 49, 50);
    grid_layout_.addWidget(&btn_render_, 49, 52, 1, 1);
    grid_layout_.addWidget(&btn_cancel_, 49, 53, 1, 1);

    connect(&btn_render_, &QPushButton::clicked, [&]()
            { renderRT(); });
    connect(&btn_cancel_, &QPushButton::clicked, [&]()
            { render_control_flag_ = 0; });

    glwidget_preview_.setFixedHeight(512);
    glwidget_preview_.setFixedWidth(512);

    grid_layout_.addWidget(&glwidget_preview_, 0, 50, 1, 10);

    camera_.pos = {50.0f, 320.0f, 500.0f};
    camera_.gaze = vec3(-0.2f, -0.5f, -1.0f).normalized();
    vec3 camera_hand = vec3(1.0f, 0.0f, 0.1f).normalized();
    camera_.up = camera_hand.cross(camera_.gaze).normalized();
    camera_.fov_h = 30.0f;
    camera_.aspect = 1.0;

    text_edit_scene_.setText("../scenes/cornell/CornellBox-Original.obj -p 0 0 0 -s 100");
    scene_loader_.fromSceneDescription("../scenes/cornell/CornellBox-Original.obj -p 0 0 0 -s 100");

    updateVertices();

    glwidget_preview_.setCamera(&camera_);

    auto &triangles = scene_loader_.getTriangles();

    renderer_ = new RendererPTLS();
    renderer_->prepare(triangles);

    grid_layout_.addWidget(&combo_renderer_, 2, 50, 1, 5);
    combo_renderer_.addItem("PathTracing");
    combo_renderer_.addItem("PathTracingNEE");
    combo_renderer_.addItem("PSSMLT");
    combo_renderer_.addItem("BDPT");
    combo_renderer_.addItem("PhotonMapping");
    combo_renderer_.addItem("NeuralRadianceCache");
    combo_renderer_.setCurrentIndex(1);

    connect(&combo_renderer_, QOverload<int>::of(&QComboBox::currentIndexChanged), [&](int id)
            {
        delete renderer_;
        if (id == 0)
        {
            renderer_ = new RendererPT;
        }
        else if (id == 1)
        {
            renderer_ = new RendererPTLS;
        }
        else if (id == 2)
        {
            renderer_ = new RendererPSSMLT;
        }
        else if (id == 3)
        {
            renderer_ = new RendererBDPT;
        }
        else if (id == 4)
        {
            renderer_ = new RendererPM;
        }
        else if (id == 5)
        {
            renderer_ = new RendererNRC;
        }
        renderer_->prepare(triangles); });

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

    connect(&line_edit_yaw_, &QLineEdit::textEdited, [&]()
            {
        if (line_edit_yaw_.text().toFloat() == camera_.toEuler()[0])
            return;
        camera_.fromEuler(line_edit_yaw_.text().toFloat(), line_edit_pitch_.text().toFloat(), line_edit_roll_.text().toFloat());
        renderRT_preview();
        glwidget_preview_.update(); });

    grid_layout_.addWidget(&line_edit_pitch_, 31, 51, 1, 4);
    grid_layout_.addWidget(&label_pitch_, 31, 50, 1, 1);
    line_edit_pitch_.setValidator(new QDoubleValidator(-1e9, 1e9, 4, this));
    line_edit_pitch_.setText(QString::number(camera_.toEuler()[1], 'f', 4));

    connect(&line_edit_pitch_, &QLineEdit::textEdited, [&]()
            {
        if (line_edit_pitch_.text().toFloat() == camera_.toEuler()[1])
            return;
        camera_.fromEuler(line_edit_yaw_.text().toFloat(), line_edit_pitch_.text().toFloat(), line_edit_roll_.text().toFloat());
        renderRT_preview();
        glwidget_preview_.update(); });

    grid_layout_.addWidget(&line_edit_roll_, 32, 51, 1, 4);
    grid_layout_.addWidget(&label_roll_, 32, 50, 1, 1);
    line_edit_roll_.setValidator(new QDoubleValidator(-1e9, 1e9, 4, this));
    line_edit_roll_.setText(QString::number(camera_.toEuler()[2], 'f', 4));

    connect(&line_edit_roll_, &QLineEdit::textEdited, [&]()
            {
        if (line_edit_roll_.text().toFloat() == camera_.toEuler()[2])
            return;
        camera_.fromEuler(line_edit_yaw_.text().toFloat(), line_edit_pitch_.text().toFloat(), line_edit_roll_.text().toFloat());
        renderRT_preview();
        glwidget_preview_.update(); });

    bindLineEdit(line_edit_envmap_, str_envmap_);
    grid_layout_.addWidget(&line_edit_envmap_, 36, 51, 1, 4);
    grid_layout_.addWidget(&label_envmap_, 36, 50, 1, 1);

    btn_load_scene_.setText("Load");
    label_setting_.setText("Custom Setting");
    grid_layout_.addWidget(&label_setting_, 40, 50, 1, 1);
    grid_layout_.addWidget(&text_edit_setting_, 41, 50, 3, 5);
    grid_layout_.addWidget(&label_scene_, 44, 50, 1, 1);
    grid_layout_.addWidget(&text_edit_scene_, 45, 50, 3, 5);
    grid_layout_.addWidget(&btn_load_scene_, 49, 50, 1, 1);
    grid_layout_.addWidget(&btn_apply_, 49, 51, 1, 1);
    grid_layout_.addWidget(&progress_bar_, 49, 0, 1, 50);

    connect(&btn_load_scene_, &QPushButton::clicked, [&]()
            {
        scene_loader_.fromSceneDescription(text_edit_scene_.toPlainText().toStdString());
        renderer_->prepare(scene_loader_.getTriangles());
        updateVertices();
        renderRT_preview(); });

    connect(&btn_apply_, &QPushButton::clicked, [&]()
            {
        str_setting_ = text_edit_setting_.toPlainText().toStdString();
        renderRT_preview(); });

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
    label_scene_.setText("Scene Desc");

    progress_bar_.setMinimum(0);
    progress_bar_.setMaximum(100 * 100);
    progress_bar_.setOrientation(Qt::Horizontal);
    progress_bar_.setValue(0);

    label_envmap_.setText("Envmap");
    line_edit_envmap_.setText("");
    str_envmap_ = line_edit_envmap_.text();

    this->setLayout(&grid_layout_);
    this->update();
}

Widget::~Widget()
{
    exit(0);
}

void Widget::resizeEvent(QResizeEvent *event)
{
    framebufferUpdated();
}

void Widget::renderRT()
{
    render_control_flag_ = 1;
    if (lock_render_.tryLock() == false)
    {
        qDebug() << "Already rendering, action ignored.";
        return;
    }
    Envmap envmap(&env_map_);
    env_map_.load(str_envmap_.toStdString());
    renderer_->setEnvmap(envmap);
    // Render
    QTime timer;
    timer.start();
    std::cout << "Loading scene..." << std::endl;
    auto &triangles = scene_loader_.getTriangles();
    std::cout << "Loading scene ok, " << timer.elapsed() * 0.001 << " secs used" << std::endl;

    Config config(str_setting_);
    config.setValueStr("imgw", std::to_string(img_width_));
    config.setValueStr("imgh", std::to_string(img_height_));
    config.setValueStr("spp", std::to_string(spp_));
    config.setValueStr("camfov", std::to_string(camera_.fov_h));
    config.setValueStr("camasp", std::to_string(camera_.aspect));

    this->renderer_->render(
        camera_, triangles, framebuffer_, config, [&](bool f)
        { framebufferUpdated(f); },
        render_control_flag_,
        [&](float p)
        { progress_ = p / 100.0f; },
        lock_framebuffer_,
        &envmap);
    lock_render_.unlock();
    framebufferUpdated();
    // compare and evaluate mse
    if (img_ref_.sizeInBytes() == 0)
    {
        img_ref_ = framebuffer_;
        img_ref_.bits();
        std::cout << "stored" << std::endl;
    }
    else
    {
        Utils utils;
        float mse = utils.mse(img_ref_, framebuffer_);
        std::cout << "MSE = " << std::fixed << std::setprecision(6) << mse << std::endl;
    }
    framebuffer_.save("./" + QTime::currentTime().toString().replace(":", "") + combo_renderer_.currentText() + QString::fromStdString(std::to_string(spp_)) +  "spp.bmp", "bmp");
}

void Widget::renderRT_preview()
{
    if (last_review_render_time_.msecsTo(QTime::currentTime()) < 300)
        return;

    if (lock_render_.tryLock() == false)
    {
        qDebug() << "Already rendering, action ignored.";
        return;
    }

    Envmap envmap(&env_map_);
    env_map_.load(str_envmap_.toStdString());
    renderer_->setEnvmap(envmap);
    render_control_flag_ = 1;
    last_review_render_time_ = QTime::currentTime();
    auto &triangles = scene_loader_.getTriangles();

    Config config(str_setting_);
    config.setValueStr("imgw", std::to_string(img_width_ / preview_level_));
    config.setValueStr("imgh", std::to_string(img_height_ / preview_level_));
    config.setValueStr("spp", std::to_string(spp_preview_));
    config.setValueStr("camfov", std::to_string(camera_.fov_h));
    config.setValueStr("camasp", std::to_string(camera_.aspect));

    this->renderer_->render(
        camera_, triangles, framebuffer_, config, [&](bool f)
        { framebufferUpdated(f); },
        render_control_flag_,
        [&](float p)
        { progress_ = p / 100.0f; },
        lock_framebuffer_,
        &envmap);
    lock_render_.unlock();
    framebufferUpdated();
}

void Widget::updateVertices()
{
    glwidget_preview_.setVertices(QVector<float>::fromStdVector(scene_loader_.getVerticesNormals()));
}

void Widget::bindLineEdit(QLineEdit &line_edit, float &var)
{
    line_edit.setValidator(new QDoubleValidator(-1e9, 1e9, 4, this));
    line_edit.setText(QString::number(var, 'f', 4));
    connect(&line_edit, &QLineEdit::textEdited, [&]()
            { bool t; float v; v = line_edit.text().toDouble(&t); if(t) var=v; renderRT_preview(); });
}

void Widget::bindLineEdit(QLineEdit &line_edit, int &var)
{
    line_edit.setValidator(new QIntValidator(-1e9, 1e9, this));
    line_edit.setText(QString::number(var));
    connect(&line_edit, &QLineEdit::textEdited, [&]()
            { bool t; int v; v = line_edit.text().toInt(&t); if(t) var=v; renderRT_preview(); });
}

void Widget::bindLineEdit(QLineEdit &line_edit, QString &var)
{
    line_edit.setText(var);
    connect(&line_edit, &QLineEdit::textEdited, [&]()
            { var = line_edit.text(); renderRT_preview(); });
}

void Widget::framebufferUpdated(bool forcing)
{
    static QTime last_time = QTime::currentTime().addMSecs(-1e9);
    if (!forcing && last_time.msecsTo(QTime::currentTime()) < 100) // Minimal refresh interval
        return;
    last_time = QTime::currentTime();

    progress_bar_.setValue(progress_ * 10000);
    QApplication::processEvents();

    int final_width = std::min(label_render_result_.width(), label_render_result_.height() * img_width_ / img_height_);
    int final_height = std::min(label_render_result_.height(), label_render_result_.width() * img_height_ / img_width_);
    int padding_width = label_render_result_.width() - final_width;
    int padding_height = label_render_result_.height() - final_height;

    if (!forcing)
    {
        if (lock_framebuffer_.tryLock(500) == false)
        {
            return;
        }
    }
    else
    {
        lock_framebuffer_.lock();
    }
    QImage framebuffer_tmp = framebuffer_.copy();
    label_render_result_.setPixmap(QPixmap::fromImage(framebuffer_tmp.scaled(QSize(final_width, final_height)).copy(-padding_width / 2, -padding_height / 2, label_render_result_.width(), label_render_result_.height())));
    label_render_result_.repaint();
    lock_framebuffer_.unlock();

    progress_bar_.setValue(progress_ * 10000);
    QApplication::processEvents();
}