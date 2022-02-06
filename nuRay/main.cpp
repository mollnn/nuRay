#include "widget.h"

#include <QApplication>
#include <QLabel>
#include <QGridLayout>
#include <QDebug>
#include <bits/stdc++.h>

#include "loader.h"
#include "camera.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    QGridLayout gl;
    QLabel l(&w);
    QImage img(QSize(512, 512), QImage::Format_RGB888);

    // Render

    Loader loader;
    loader.loadObj("rock.obj", {0.0f, 0.0f, 0.0f}, 1.0f);

    auto triangles = loader.getTriangles();

    Camera camera;
    camera.pos = {0.0f, 0.0f, 3.0f};
    camera.gaze = {0.0f, 0.0f, -1.0f};
    camera.up = {0.0f, 1.0f, 0.0f};
    camera.img_width = 512;
    camera.img_height = 512;
    camera.fov_h = 90.0f * 3.14159f / 180.0f;
    camera.aspect = 1.0;

    for(int y=0;y<camera.img_height;y++)
    {
        for(int x=0;x<camera.img_width;x++)
        {
            vec3 ray_dir = camera.generateRay(x,y);
            float min_t = 5;
            for(auto triangle: triangles)
            {
                auto [t, b1, b2] = triangle.intersection(camera.pos, ray_dir);
                if(t<min_t && t>0 && b1 >0 && b2>0 && b1+b2<1)
                {
                    min_t = t;
                }
            }
            // img.setPixel(x,y,qRgb(ray_dir[0] * 127 + 128,ray_dir[1] * 127 + 128,ray_dir[2] * 127 + 128));
            img.setPixel(x,y,qRgb(min_t * 50,min_t * 50,min_t * 50));
        }
    }

    // Display 

    l.setFixedSize(QSize(512, 512));
    l.setPixmap(QPixmap::fromImage(img));
    gl.addWidget(&l, 0, 0, 1, 1);
    w.setLayout(&gl);
    w.show();

    return a.exec();
}
