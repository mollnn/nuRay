#ifndef __TRIANGLE_H__
#define __TRIANGLE_H__

#include <bits/stdc++.h>
#include "vec3.h"

struct Triangle
{
    vec3 p[3];
    vec3 n[3];
    vec3 t[3];

    Triangle();
    Triangle(const vec3& p0, const vec3& p1, const vec3& p2);

    void evalNormal();
    std::tuple<float, float, float> intersection(const vec3 &o, const vec3 &d) const;
    vec3 getNormal(float b1, float b2)const;
    vec3 getTexCoords(float b1, float b2)const;
};

std::ostream &operator<<(std::ostream& lhs, const Triangle &rhs);

#endif