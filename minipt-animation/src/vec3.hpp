#ifndef __VEC3_HPP__
#define __VEC3_HPP__

#include <bits/stdc++.h>
using namespace std;

struct vec3
{
    double x, y, z;
    vec3 operator+(const vec3 &rhs) const { return {x + rhs.x, y + rhs.y, z + rhs.z}; }
    vec3 operator-(const vec3 &rhs) const { return {x - rhs.x, y - rhs.y, z - rhs.z}; }
    vec3 operator*(const vec3 &rhs) const { return {x * rhs.x, y * rhs.y, z * rhs.z}; }
    vec3 operator*(double rhs) const { return {x * rhs, y * rhs, z * rhs}; }
    vec3 operator/(double rhs) const { return {x / rhs, y / rhs, z / rhs}; }
    double dot(const vec3 &rhs) const { return x * rhs.x + y * rhs.y + z * rhs.z; }
    vec3 cross(const vec3 &rhs) const { return {y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x}; }
    double norm2() const { return x * x + y * y + z * z; }
    double norm() const { return sqrt(norm2()); }
    vec3 unit() const { return (*this) / norm(); }
    double avg() const { return (x + y + z) / 3; }
};
vec3 operator*(double lhs, const vec3 &rhs) { return {lhs * rhs.x, lhs * rhs.y, lhs * rhs.z}; }

struct vec3_uint8
{
    uint8_t x, y, z;
    vec3_uint8 operator+(const vec3_uint8 &rhs) const { return {x + rhs.x, y + rhs.y, z + rhs.z}; }
    vec3_uint8 operator-(const vec3_uint8 &rhs) const { return {x - rhs.x, y - rhs.y, z - rhs.z}; }
    vec3_uint8 operator*(const vec3_uint8 &rhs) const { return {x * rhs.x, y * rhs.y, z * rhs.z}; }
    vec3_uint8 operator*(uint8_t rhs) const { return {x * rhs, y * rhs, z * rhs}; }
    vec3_uint8 operator/(uint8_t rhs) const { return {x / rhs, y / rhs, z / rhs}; }
    uint8_t dot(const vec3_uint8 &rhs) const { return x * rhs.x + y * rhs.y + z * rhs.z; }
    vec3_uint8 cross(const vec3_uint8 &rhs) const { return {y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x}; }
    uint8_t norm2() const { return x * x + y * y + z * z; }
    uint8_t norm() const { return sqrt(norm2()); }
    vec3_uint8 unit() const { return (*this) / norm(); }
    uint8_t avg() const { return (x + y + z) / 3; }
};
vec3_uint8 operator*(uint8_t lhs, const vec3_uint8 &rhs) { return {lhs * rhs.x, lhs * rhs.y, lhs * rhs.z}; }

#endif