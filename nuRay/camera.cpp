#include "camera.h"

vec3 Camera::generateRay(float img_x, float img_y)
{
    float canonical_x = img_x / img_width * 2 - 1;
    float canonical_y = img_y / img_height * 2 - 1;
    vec3 hand = gaze.cross(up).normalized();
    float film_width = 2 * tan(fov_h / 2);
    float film_height = film_width / aspect;
    return (gaze + hand * film_width * canonical_x - up * film_height * canonical_y).normalized();
}