#ifndef __VEC3_H__
#define __VEC3_H__

#include <bits/stdc++.h>

struct vec3
{
    float v[3];

    vec3();
    vec3(float x, float y, float z);

    vec3 &operator+=(const vec3 &rhs);
    vec3 &operator-=(const vec3 &rhs);
    vec3 &operator*=(const vec3 &rhs);
    vec3 &operator*=(float rhs);
    vec3 &operator/=(float rhs);

    vec3 operator-() const;

    vec3 operator+(const vec3 &rhs) const;
    vec3 operator-(const vec3 &rhs) const;
    vec3 operator*(const vec3 &rhs) const;
    vec3 operator*(float rhs) const;
    vec3 operator/(float rhs) const;
    float dot(const vec3 &rhs) const;
    vec3 cross(const vec3 &rhs) const;
    float norm2() const;
    float norm() const;
    vec3 normalized() const;
    float avg() const;

    float &operator[](int i);
    float operator[](int i) const;
};
vec3 operator*(float lhs, const vec3 &rhs);
std::ostream &operator<<(std::ostream& lhs, const vec3 &rhs);

#endif