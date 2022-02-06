#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "vec3.h"
#include <cmath>

struct Camera
{
    vec3 pos;
    vec3 gaze;
    vec3 up;

    float fov_h;  // horizon fov
    float aspect; // width / height

    int img_width;
    int img_height;

    vec3 generateRay(float img_x, float img_y);
};



#endif
