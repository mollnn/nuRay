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

    vec3 generateRay(float img_x, float img_y, float img_width, float img_height) const;

    void turnH(float k);
    void turnV(float k);
    void go(float x, float y, float z);

    vec3 toEuler() const;
    void fromEuler(const vec3 &euler);
    void fromEuler(float yaw, float pitch, float roll);

    float filmSize() const;
};

#endif
