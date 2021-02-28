#ifndef _TRIANGLE_HPP_
#define _TRIANGLE_HPP_

#include "vec3.hpp"
#include "material.hpp"
#include "fastmath.hpp"


class Triangle
{
public:
    vec3 p0, p1, p2;
    Material mat;
    vec3 n;
    vec3 math_normal() const { return ((p1 - p0).cross(p2 - p0)).unit(); }
    vec3 normal() const { return n; }
    void auto_normal() { n = math_normal(); }
    std::pair<double, vec3> intersect(vec3 pos, vec3 dir) const
    {
        vec3 tpos = p0;
        vec3 tdir = n;
        if (fabs(tdir.dot(dir)) < eps)
            return {-1, vec3()};
        if (tdir.dot(dir) < 0)
            tdir = -1 * tdir;
        double x = (tpos - pos).dot(tdir) / (tdir.dot(dir));
        if (x < 0)
            return {-1, vec3()};
        vec3 ph = pos + x * dir;
        vec3 q0 = (p1 - p0).cross(ph - p0);
        vec3 q1 = (p2 - p1).cross(ph - p1);
        vec3 q2 = (p0 - p2).cross(ph - p2);
        if (q0.dot(q1) > 0 && q1.dot(q2) > 0 && q2.dot(q0) > 0)
        {
            return {x, ph};
        }
        else
            return {-1, vec3()};
    }
};

std::tuple<double, vec3, const Triangle *> Intersect(const std::vector<Triangle> &triangles, vec3 pos, vec3 dir)
{
    double hitdis = 2e18;
    vec3 hitpos;
    const Triangle *hitobj = NULL;
    for (auto &triangle : triangles)
    {
        auto [thdis, thpos] = triangle.intersect(pos, dir);
        if (thdis > 0 && thdis < hitdis)
        {
            hitdis = thdis;
            hitpos = thpos;
            hitobj = &triangle;
        }
    }
    if (hitdis > 1e18)
        return {-1, hitpos, hitobj};
    return {hitdis, hitpos, hitobj};
}

#endif