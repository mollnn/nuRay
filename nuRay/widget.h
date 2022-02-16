#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

#include <QGridLayout>
#include <QDebug>
#include <bits/stdc++.h>

#include "loader.h"
#include "camera.h"
#include "renderer.h"
#include "material.h"
#include "matlambert.h"
#include "matblinnphong.h"
#include "matlight.h"
#include <QTime>
#include <QLabel>
#include <QPushButton>
#include "glwidget.h"
#include <QLineEdit>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    void renderRT();
    void renderRT_preview();
    void updateVertices();

private:
    Ui::Widget *ui;

    QGridLayout grid;
    QLabel l;
    Camera camera;
    QImage render_result;
    Loader loader;
    std::vector<Material *> custom_materials;

    QPushButton btn_render;
    GlWidget glw;

    Renderer renderer;

    QLineEdit line_edit_cam_pos_x, line_edit_cam_pos_y, line_edit_cam_pos_z;
    QLineEdit line_edit_spp, line_edit_spp_preview;
    QLineEdit line_edit_yaw, line_edit_pitch, line_edit_roll;
    QLineEdit line_edit_fov_h, line_edit_aspect, line_edit_img_w, line_edit_img_h, line_edit_preview_level;

    QLabel label_cam_pos_x, label_cam_pos_y, label_cam_pos_z;
    QLabel label_spp, label_spp_preview;
    QLabel label_yaw, label_pitch, label_roll;
    QLabel label_fov_h, label_aspect, label_img_w, label_img_h, label_preview_level;
    int spp = 32, spp_preview = 8;
    int img_width = 256, img_height = 256, preview_level = 8;

    void bindLineEdit(QLineEdit &line_edit, float &var);
    void bindLineEdit(QLineEdit &line_edit, int &var);

    QTime last_update;
};
#endif // WIDGET_H
