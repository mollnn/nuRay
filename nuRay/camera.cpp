#include "camera.h"

vec3 Camera::generateRay(float img_x, float img_y, float img_width, float img_height) const
{
    float canonical_x = img_x / img_width * 2 - 1;
    float canonical_y = img_y / img_height * 2 - 1;
    vec3 hand = gaze.cross(up).normalized();
    float film_height = 2 * tan(fov_h * 3.14159 / 180 / 2);
    float film_width = film_height * aspect;
    return (gaze + hand * film_width * canonical_x * 0.5 - up * film_height * canonical_y * 0.5).normalized();
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

vec3 Camera::toEuler() const
{
    vec3 hand = gaze.cross(up).normalized();
    float m13 = -gaze[0] + 1e-9f;
    float m21 = hand[1] + 1e-9f;
    float m22 = up[1] + 1e-9f;
    float m23 = -gaze[1] + 1e-9f;
    float m33 = -gaze[2] + 1e-9f;
    return {atan2(m13, m33) * 180 / 3.14159, asin(-m23) * 180 / 3.14159, atan2(m21, m22) * 180 / 3.14159};
}

void Camera::fromEuler(const vec3 &euler)
{
    float c1 = cos(euler[0] / 180 * 3.14159), s1 = sin(euler[0] / 180 * 3.14159);
    float c2 = cos(euler[1] / 180 * 3.14159), s2 = sin(euler[1] / 180 * 3.14159);
    float c3 = cos(euler[2] / 180 * 3.14159), s3 = sin(euler[2] / 180 * 3.14159);
    float m11 = c1 * c3 + s1 * s2 * s3;
    float m12 = c3 * s1 * s2 - c1 * s3;
    float m13 = c2 * s1;
    float m21 = c2 * s3;
    float m22 = c2 * c3;
    float m23 = -s2;
    float m31 = c1 * s2 * s3 - s1 * c3;
    float m32 = s1 * s3 + c1 * c3 * s2;
    float m33 = c1 * c2;
    up = {m12, m22, m32};
    gaze = {-m13, -m23, -m33};
    std::cout << gaze << " " << up << std::endl;
}

void Camera::fromEuler(float a, float b, float c)
{
    fromEuler({a, b, c});
}