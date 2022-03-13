#ifndef __TRIANGLE_H__
#define __TRIANGLE_H__

#include <bits/stdc++.h>
#include "../math/vec3.h"
#include "../material/material.h"

struct Triangle
{
    vec3 p[3];
    vec3 t[3];
    vec3 n[3];
    const Material* mat;

    Triangle();
    Triangle(const vec3& p0, const vec3& p1, const vec3& p2, 
        const vec3& t0 = vec3(0.0f, 0.0f, 0.0f), 
        const vec3& t1 = vec3(0.0f, 0.0f, 0.0f), 
        const vec3& t2 = vec3(0.0f, 0.0f, 0.0f), 
        const vec3& n0 = vec3(0.0f, 0.0f, 0.0f), 
        const vec3& n1 = vec3(0.0f, 0.0f, 0.0f), 
        const vec3& n2 = vec3(0.0f, 0.0f, 0.0f), 
        const Material* mat = nullptr
    );

    void evalNormal();
    std::tuple<float, float, float> intersection(const vec3 &o, const vec3 &d) const;
    vec3 getNormal(float b1, float b2)const;
    vec3 getTexCoords(float b1, float b2)const;

    std::tuple<vec3, float, float> sample(Sampler& sampler) const;
    float area() const;

    vec3 pMin() const;
    vec3 pMax() const;
};

std::ostream &operator<<(std::ostream& lhs, const Triangle &rhs);

#endif