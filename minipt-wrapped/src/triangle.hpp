#ifndef _TRIANGLE_HPP_
#define _TRIANGLE_HPP_

#include "vec3.hpp"
#include "material.hpp"

class Triangle
{
public:
    vec3 p0, p1, p2;
    Material mat;
    vec3 n;
    vec3 math_normal() const;
    vec3 normal() const;
    void auto_normal();
    std::pair<double, vec3> intersect(vec3 pos, vec3 dir) const;
};

std::tuple<double, vec3, const Triangle *> Intersect(const std::vector<Triangle> &triangles, vec3 pos, vec3 dir);

#endif