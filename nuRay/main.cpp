#include "widget.h"

#include <QApplication>
#include <QLabel>
#include <QGridLayout>
#include <QDebug>
#include <bits/stdc++.h>

#include "loader.h"
#include "camera.h"
#include "renderer.h"
#include "material.h"
#include "matlambert.h"
#include "matlight.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    QGridLayout gl;
    QLabel l(&w);

    const int RSIZE = 64;
    QImage img(QSize(RSIZE, RSIZE), QImage::Format_RGB888);

    // Render

    std::vector<Material *> user_materials;
    user_materials.push_back(new MatLight(vec3(1.0, 1.0, 1.0) * 10));
    user_materials.push_back(new MatLambert(vec3(0.2, 0.1, 0.1)));
    user_materials.push_back(new MatLambert(vec3(0.7, 0.7, 0.7)));

    qDebug() << "Loading...";

    Loader loader;
    // loader.loadObj("spot/spot.obj", {0.0f, 1.0f, 0.0f}, 1.0f);
    // loader.loadObj("cube/cube.obj", {0.0f, -5.0f, 0.0f}, 5.0f, user_materials[2]);
    // loader.loadObj("cube/cube.obj", {0.0f, 10.0f, 0.0f}, 5.0f, user_materials[2]);
    // loader.loadObj("cube/cube.obj", {-10.0f, 0.0f, 0.0f}, 5.0f, user_materials[2]);
    // loader.loadObj("cube/cube.obj", {10.0f, 0.0f, 0.0f}, 5.0f, user_materials[2]);
    // loader.loadObj("cube/cube.obj", {0.0f, 0.0f, -10.0f}, 5.0f, user_materials[2]);
    // loader.loadObj("cube/cube.obj", {0.0f, 0.0f, 10.0f}, 5.0f, user_materials[2]);
    // loader.loadObj("test/test.obj", {0.0f, 4.9f, 0.0f}, 2.0f);
    // loader.loadObj("cube.obj", {0.0f, 0.5f, 0.0f}, 1.0f);

    loader.loadObj("test/test.obj", {0.0f, 10.0f, 0.0f}, 5.0f);
    loader.loadObj("sponza/sponza.obj", {0.0f, 0.0f, 0.0f}, 1.0f);

    auto triangles = loader.getTriangles();

    Camera camera;
    camera.pos = {0.0f, 1.0f, 0.0f};
    camera.gaze = vec3(0.0f, 0.0f, 1.0f).normalized();
    camera.up = {0.0f, 1.0f, 0.0f};
    camera.img_width = RSIZE;
    camera.img_height = RSIZE;
    camera.fov_h = 90.0f * 3.14159f / 180.0f;
    camera.aspect = 1.0;

    l.setFixedSize(QSize(1024, 1024));
    gl.addWidget(&l, 0, 0, 1, 1);
    w.setLayout(&gl);
    w.show();

    Renderer renderer;


    qDebug() << "Rendering...";

    renderer.render(camera, triangles, img);
    l.setPixmap(QPixmap::fromImage(img.scaled(QSize(1024, 1024))));

    // Clear

    // for (auto i : materials)
    //     delete i;

    // Display

    return a.exec();
}
