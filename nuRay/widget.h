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

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    void renderRT();

private:
    Ui::Widget *ui;

    QGridLayout grid;
    QLabel l;
    Camera camera;
    QImage render_result;

    QPushButton btn_render;
};
#endif // WIDGET_H
