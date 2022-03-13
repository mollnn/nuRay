#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

#include <QGridLayout>
#include <QDebug>
#include <bits/stdc++.h>

#include "../scene/loader.h"
#include "../scene/camera.h"
#include "../renderer/renderer.h"
#include "../renderer/rendererpt.h"
#include "../material/material.h"
#include "../material/matlambert.h"
#include "../material/matblinnphong.h"
#include "../material/matlight.h"
#include <QTime>
#include <QLabel>
#include <QPushButton>
#include "../gui/glwidget.h"
#include <QLineEdit>
#include <QTextEdit>
#include <QProgressBar>
#include "../scene/texture.h"
#include <QMutex>
#include <QComboBox>

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    virtual void resizeEvent(QResizeEvent *event) override;

    void renderRT();
    void renderRT_preview();
    void updateVertices();

    void framebufferUpdated(bool forcing = true);

private:
    QGridLayout grid_layout_;
    QLabel label_render_result_;
    Camera camera_;
    QImage framebuffer_;
    Loader scene_loader_;
    QPushButton btn_render_;
    QPushButton btn_cancel_;
    GlWidget glwidget_preview_;
    Renderer* renderer_;
    QLineEdit line_edit_cam_pos_x_, line_edit_cam_pos_y_, line_edit_cam_pos_z_;
    QLineEdit line_edit_spp_, line_edit_spp_preview_;
    QLineEdit line_edit_yaw_, line_edit_pitch_, line_edit_roll_;
    QLineEdit line_edit_fov_h_, line_edit_aspect_, line_edit_img_w_, line_edit_img_h_, line_edit_preview_level_;
    QLineEdit line_edit_envmap_;
    QLabel label_cam_pos_x_, label_cam_pos_y_, label_cam_pos_z_;
    QLabel label_spp_, label_spp_preview_;
    QLabel label_yaw_, label_pitch_, label_roll_;
    QLabel label_fov_h_, label_aspect_, label_img_w_, label_img_h_, label_preview_level_;
    QLabel label_envmap_;
    QLabel label_scene_;
    int spp_ = 32, spp_preview_ = 8;
    int img_width_ = 128, img_height_ = 128, preview_level_ = 8;
    QTime last_review_render_time_;
    QTextEdit text_edit_scene_;
    QPushButton btn_load_scene_;
    Texture env_map_;
    std::atomic<int> render_control_flag_;
    std::atomic<float> progress_;
    QMutex lock_framebuffer_;
    QMutex lock_render_;
    QProgressBar progress_bar_;
    QString str_envmap_;
    QComboBox combo_renderer_;

    void bindLineEdit(QLineEdit &line_edit, float &var);
    void bindLineEdit(QLineEdit &line_edit, int &var);
    void bindLineEdit(QLineEdit &line_edit, QString &var);
};
#endif // WIDGET_H
