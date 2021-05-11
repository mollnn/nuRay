#include <bits/stdc++.h>
using namespace std;

#include "vec3.hpp"
#include "image.hpp"
#include "timer.hpp"
#include "color.hpp"
#include "random.hpp"
#include "material.hpp"
#include "triangle.hpp"
#include "scene.hpp"
#include "pathtracer.hpp"
#include "renderer.hpp"
#include "testscene.hpp"

class Minipt
{
public:
    Scene scene;
    TestScene test_scene;

    int img_siz_x = 1024;
    double img_aspect = 2.39;
    int img_siz_y = img_siz_x / img_aspect;
    int spp = 16;
    vec3 cam_dir = (vec3){0, 1, -1}.unit();
    vec3 cam_pos = {0, -5, 5};
    vec3 cam_top = {0, 1, 1};
    double focal = 24;
    double near_clip = 0.1;

    Minipt();

    void LoadDefaultLightset();

    void AutoNormal();

    Image Render();
};