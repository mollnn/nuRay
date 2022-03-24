#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "../math/vec3.h"
#include <cmath>

struct Camera
{
    vec3 pos;
    vec3 gaze;
    vec3 up;

    float fov_h;  // horizon fov
    float aspect; // width / height

    Camera();
    Camera(float fov, float asp, vec3 p, vec3 g, vec3 u);
    Camera(float fov, float asp, vec3 p, float yaw, float pitch, float roll);

    vec3 generateRay(float img_x, float img_y, float img_width, float img_height) const;
    std::pair<int,int> getCoord(vec3 dir, float img_width, float img_height) const;

    void turnH(float k);
    void turnV(float k);
    void go(float x, float y, float z);

    vec3 toEuler() const;
    void fromEuler(const vec3 &euler);
    void fromEuler(float yaw, float pitch, float roll);

    float filmSize() const;
};

#endif
