#ifndef __MAT4_H__
#define __MAT4_H__

#include <bits/stdc++.h>
#include <vec4.h>

struct mat4
{
    vec4 v[4]; // ! column-primary

    mat4();
    mat4(float val);
    mat4(const vec4 &x, const vec4 &y, const vec4 &z, const vec4 &w);

    mat4 &operator+=(const mat4 &rhs);
    mat4 &operator-=(const mat4 &rhs);

    mat4 &operator*=(const mat4 &rhs);

    mat4 &operator*=(float rhs);
    mat4 &operator/=(float rhs);

    mat4 operator-() const;

    mat4 operator+(const mat4 &rhs) const;
    mat4 operator-(const mat4 &rhs) const;

    mat4 operator*(const mat4 &rhs) const;
    vec4 operator*(const vec4 &rhs) const;

    mat4 operator*(float rhs) const;
    mat4 operator/(float rhs) const;

    float& m(int i, int j);
    float m(int i, int j) const;
    mat4 T() const;

    vec4 &operator[](int i);
    vec4 operator[](int i) const;
};

vec4 operator*(const vec4 &lhs, const mat4 &rhs);
std::ostream &operator<<(std::ostream &lhs, const mat4 &rhs);

#endif