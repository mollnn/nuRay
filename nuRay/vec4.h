#ifndef __VEC4_H__
#define __VEC4_H__

#include <bits/stdc++.h>
#include <vec3.h>

struct vec4
{
    float v[4];

    vec4();
    vec4(float i);
    vec4(float x, float y, float z, float w = 1.0f);
    vec4(const vec3& x, float w = 1.0f);
    vec3 xyz() const;

    vec4 &operator+=(const vec4 &rhs);
    vec4 &operator-=(const vec4 &rhs);
    vec4 &operator*=(const vec4 &rhs);
    vec4 &operator*=(float rhs);
    vec4 &operator/=(float rhs);

    vec4 operator-() const;

    vec4 operator+(const vec4 &rhs) const;
    vec4 operator-(const vec4 &rhs) const;
    vec4 operator*(const vec4 &rhs) const;
    vec4 operator*(float rhs) const;
    vec4 operator/(float rhs) const;
    float dot(const vec4 &rhs) const;
    float norm2() const;
    float norm() const;
    vec4 normalized() const;
    float avg() const;
    vec4 pow(float q) const;

    float &operator[](int i);
    float operator[](int i) const;
};
vec4 operator*(float lhs, const vec4 &rhs);
std::ostream &operator<<(std::ostream& lhs, const vec4 &rhs);
vec4 min(const vec4& a, const vec4& b);
vec4 max(const vec4& a, const vec4& b);

#endif