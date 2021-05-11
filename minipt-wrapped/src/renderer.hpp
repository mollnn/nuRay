#ifndef _RENDERER_HPP_
#define _RENDERER_HPP_

#include <bits/stdc++.h>
using namespace std;

#include "image.hpp"
#include "pathtracer.hpp"

void RenderThread(int img_siz_x, int img_siz_y, int spp, vec3 cam_pos, vec3 cam_dir, vec3 cam_top, double focal, double near_clip,
                  Image &image, const Scene &scene, double fp_siz_x, double fp_siz_y, vec3 fp_e_x, vec3 fp_e_y,
                  int img_y_min, int img_y_max);

void RenderMain(int img_siz_x, int img_siz_y, int spp, vec3 cam_pos, vec3 cam_dir, vec3 cam_top, double focal, double near_clip,
                Image &image, const Scene &scene);

#endif