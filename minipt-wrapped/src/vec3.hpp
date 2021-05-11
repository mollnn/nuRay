#ifndef __VEC3_HPP__
#define __VEC3_HPP__

#include <bits/stdc++.h>
using namespace std;

struct vec3
{
    double x, y, z;
    vec3 operator+(const vec3 &rhs) const;
    vec3 operator-(const vec3 &rhs) const;
    vec3 operator*(const vec3 &rhs) const;
    vec3 operator*(double rhs) const;
    vec3 operator/(double rhs) const;
    double dot(const vec3 &rhs) const;
    vec3 cross(const vec3 &rhs) const;
    double norm2() const;
    double norm() const;
    vec3 unit() const;
    double avg() const;
};
vec3 operator*(double lhs, const vec3 &rhs);

#endif