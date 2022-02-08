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
    QImage img(QSize(32, 32), QImage::Format_RGB888);

    // Render

    std::vector<Material *> materials;
    materials.push_back(new MatLight(vec3(1.0,1.0,1.0)*10));
    materials.push_back(new MatLambert(vec3(0.4, 0.2, 0.2)));
    materials.push_back(new MatLambert(vec3(0.2, 0.2, 0.2)));

    Loader loader;
    loader.loadObj("rock.obj", {0.0f, 0.0f, 0.0f}, 1.0f, materials[1]);
    loader.loadObj("cube.obj", {0.0f, -5.0f, 0.0f}, 5.0f, materials[2]);
    loader.loadObj("test.obj", {0.0f, 3.0f, 0.0f}, 1.0f, materials[0]);
    // loader.loadObj("cube.obj", {0.0f, 0.5f, 0.0f}, 1.0f);

    auto triangles = loader.getTriangles();

    Camera camera;
    camera.pos = {0.0f, 0.5f, 3.0f};
    camera.gaze = {0.0f, 0.0f, -1.0f};
    camera.up = {0.0f, 1.0f, 0.0f};
    camera.img_width = 32;
    camera.img_height = 32;
    camera.fov_h = 90.0f * 3.14159f / 180.0f;
    camera.aspect = 1.0;

    Renderer renderer;
    renderer.render(camera, triangles, img);

    // Clear

    for (auto i : materials)
        delete i;
 
    // Display

    l.setFixedSize(QSize(512, 512));
    l.setPixmap(QPixmap::fromImage(img.scaled(QSize(512, 512))));
    gl.addWidget(&l, 0, 0, 1, 1);
    w.setLayout(&gl);
    w.show();

    return a.exec();
}
