#include "camera.h"

vec3 Camera::generateRay(float img_x, float img_y) const
{
    float canonical_x = img_x / img_width * 2 - 1;
    float canonical_y = img_y / img_height * 2 - 1;
    vec3 hand = gaze.cross(up).normalized();
    float film_width = 2 * tan(fov_h / 2);
    float film_height = film_width / aspect;
    return (gaze + hand * film_width * canonical_x - up * film_height * canonical_y).normalized();
}

void Camera::turnH(float k)
{
    vec3 hand = gaze.cross(up).normalized();
    vec3 tmp = gaze + k * hand;
    gaze = tmp.normalized();
}

void Camera::turnV(float k)
{
    vec3 hand = gaze.cross(up).normalized();
    vec3 tmp = gaze + k * up;
    gaze = tmp.normalized();
    up = hand.cross(gaze);
}

void Camera::go(float x, float y, float z)
{
    vec3 hand = gaze.cross(up).normalized();
    pos += x * hand + y * up + z * gaze;
}
