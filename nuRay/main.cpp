#include "widget.h"

#include <QApplication>
#include <QLabel>
#include <QGridLayout>
#include <QDebug>
#include <bits/stdc++.h>

#include "loader.h"
#include "camera.h"
#include "renderer.h"   

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    QGridLayout gl;
    QLabel l(&w);
    QImage img(QSize(128, 128), QImage::Format_RGB888);

    // Render

    Loader loader;
    // loader.loadObj("rock.obj", {0.0f, 0.0f, 0.0f}, 1.0f);
    loader.loadObj("cube.obj", {0.0f, -5.0f, 0.0f}, 5.0f);
    loader.loadObj("cube.obj", {0.0f, 0.5f, 0.0f}, 1.0f);

    auto triangles = loader.getTriangles();

    Camera camera;
    camera.pos = {0.0f, 0.5f, 3.0f};
    camera.gaze = {0.0f, 0.0f, -1.0f};
    camera.up = {0.0f, 1.0f, 0.0f};
    camera.img_width = 128;
    camera.img_height = 128;
    camera.fov_h = 90.0f * 3.14159f / 180.0f;
    camera.aspect = 1.0;

    Renderer renderer;
    renderer.render(camera, triangles, img);

    // Display 

    l.setFixedSize(QSize(512, 512));
    l.setPixmap(QPixmap::fromImage(img.scaled(QSize(512,512))));
    gl.addWidget(&l, 0, 0, 1, 1);
    w.setLayout(&gl);
    w.show();

    return a.exec();
}
