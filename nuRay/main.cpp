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
#include "matblinnphong.h"
#include "matlight.h"
#include <QTime>

int main(int argc, char *argv[])
{
    // Test energy conservation
    MatBlinnPhong mbp(0.5f, 0.5f, 1000.0f);
    float ans = 0;
    for (int i = 0; i < 1000000; i++)
    {
        vec3 n(0.0f, 1.0f, 0.0f);
        vec3 wo(0.0f, 1.0f, 0.0f);
        vec3 wi = mbp.sampleBxdf(wo, n);
        float pdf = mbp.pdf(wo, n, wi);
        vec3 bxdf = mbp.bxdf(wo, n, wi, {0.0f, 0.0f, 0.0f});
        ans += bxdf[0] / pdf * n.dot(wi);
    }
    qDebug() << ans;

    QApplication a(argc, argv);
    Widget w;
    QGridLayout gl;
    QLabel l(&w);

    const int RSIZE = 32;
    QImage img(QSize(RSIZE, RSIZE), QImage::Format_RGB888);

    // Render

    std::vector<Material *> user_materials;
    user_materials.push_back(new MatLight(vec3(1.0, 1.0, 1.0) * 10));
    user_materials.push_back(new MatLambert(vec3(0.2, 0.1, 0.1)));
    user_materials.push_back(new MatLambert(vec3(0.7, 0.7, 0.7)));

    QTime timer;
    timer.start();
    std::cout << "Loading scene..." << std::endl;

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

    loader.loadObj("test/test2.obj", {0.0f, 1000.0f, 0.0f}, 30.0f, user_materials[0]);
    loader.loadObj("mitsuba/mitsuba.obj", {0.0f, 0.0f, 0.0f}, 100.0f);

    auto triangles = loader.getTriangles();

    std::cout << "Loading scene ok, " << timer.elapsed() * 0.001 << " secs used" << std::endl;

    Camera camera;
    camera.pos = {100.0f, 300.0f, 500.0f};
    camera.gaze = vec3(-0.2f, -0.5f, -1.0f).normalized();
    vec3 camera_hand = vec3(1.0f, 0.0f, 0.1f).normalized();
    camera.up = camera_hand.cross(camera.gaze).normalized();
    camera.img_width = RSIZE;
    camera.img_height = RSIZE;
    camera.fov_h = 20.0f * 3.14159f / 180.0f;
    camera.aspect = 1.0;

    l.setFixedSize(QSize(1280, 1280));
    gl.addWidget(&l, 0, 0, 1, 1);
    w.setLayout(&gl);
    w.show();

    Renderer renderer;

    renderer.render(camera, triangles, img);
    l.setPixmap(QPixmap::fromImage(img.scaled(QSize(1280, 1280))));

    // Clear

    // for (auto i : materials)
    //     delete i;

    // Display

    return a.exec();
}
