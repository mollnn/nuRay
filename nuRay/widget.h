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
#include <QTextEdit>


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
    QGridLayout grid_layout_;
    QLabel label_render_result_;
    Camera camera_;
    QImage img_render_result_;
    Loader scene_loader_;
    QPushButton btn_render_;
    GlWidget glwidget_preview_;
    Renderer renderer_;
    QLineEdit line_edit_cam_pos_x_, line_edit_cam_pos_y_, line_edit_cam_pos_z_;
    QLineEdit line_edit_spp_, line_edit_spp_preview_;
    QLineEdit line_edit_yaw_, line_edit_pitch_, line_edit_roll_;
    QLineEdit line_edit_fov_h_, line_edit_aspect_, line_edit_img_w_, line_edit_img_h_, line_edit_preview_level_;
    QLabel label_cam_pos_x_, label_cam_pos_y_, label_cam_pos_z_;
    QLabel label_spp_, label_spp_preview_;
    QLabel label_yaw_, label_pitch_, label_roll_;
    QLabel label_fov_h_, label_aspect_, label_img_w_, label_img_h_, label_preview_level_;
    QLabel label_scene_;
    int spp_ = 32, spp_preview_ = 8;
    int img_width_ = 256, img_height_ = 256, preview_level_ = 8;
    QTime last_review_render_time_;
    QTextEdit text_edit_scene_;
    QPushButton btn_load_scene_;
    

    void bindLineEdit(QLineEdit &line_edit, float &var);
    void bindLineEdit(QLineEdit &line_edit, int &var);

};
#endif // WIDGET_H
