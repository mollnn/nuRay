#include "vec3.h"
#include <cmath>

vec3::vec3() : v{0.0f, 0.0f, 0.0f} {}
vec3::vec3(float x) : v{x, x, x} {}
vec3::vec3(float x, float y, float z) : v{x, y, z} {}

vec3 &vec3::operator+=(const vec3 &rhs)
{
    return *this = {v[0] + rhs.v[0], v[1] + rhs.v[1], v[2] + rhs.v[2]};
}
vec3 &vec3::operator-=(const vec3 &rhs)
{
    return *this = {v[0] - rhs.v[0], v[1] - rhs.v[1], v[2] - rhs.v[2]};
}
vec3 &vec3::operator*=(const vec3 &rhs)
{
    return *this = {v[0] * rhs.v[0], v[1] * rhs.v[1], v[2] * rhs.v[2]};
}
vec3 &vec3::operator*=(float rhs)
{
    return *this = {v[0] * rhs, v[1] * rhs, v[2] * rhs};
}
vec3 &vec3::operator/=(float rhs)
{
    return *this = {v[0] / rhs, v[1] / rhs, v[2] / rhs};
}

vec3 vec3::operator-() const
{
    return {-v[0], -v[1], -v[2]};
}

vec3 vec3::operator+(const vec3 &rhs) const
{
    return {v[0] + rhs.v[0], v[1] + rhs.v[1], v[2] + rhs.v[2]};
}
vec3 vec3::operator-(const vec3 &rhs) const
{
    return {v[0] - rhs.v[0], v[1] - rhs.v[1], v[2] - rhs.v[2]};
}
vec3 vec3::operator*(const vec3 &rhs) const
{
    return {v[0] * rhs.v[0], v[1] * rhs.v[1], v[2] * rhs.v[2]};
}
vec3 vec3::operator*(float rhs) const
{
    return {v[0] * rhs, v[1] * rhs, v[2] * rhs};
}
vec3 vec3::operator/(float rhs) const
{
    return {v[0] / rhs, v[1] / rhs, v[2] / rhs};
}
float vec3::dot(const vec3 &rhs) const
{
    return v[0] * rhs.v[0] + v[1] * rhs.v[1] + v[2] * rhs.v[2];
}
vec3 vec3::cross(const vec3 &rhs) const
{
    return {v[1] * rhs.v[2] - v[2] * rhs.v[1], v[2] * rhs.v[0] - v[0] * rhs.v[2], v[0] * rhs.v[1] - v[1] * rhs.v[0]};
}
float vec3::norm2() const
{
    return v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
}
float vec3::norm() const
{
    return sqrt(norm2());
}
vec3 vec3::normalized() const
{
    return (*this) / norm();
}
float vec3::avg() const
{
    return (v[0] + v[1] + v[2]) / 3;
}
vec3 operator*(float lhs, const vec3 &rhs)
{
    return rhs * lhs;
}

float &vec3::operator[](int i)
{
    return v[i];
}

float vec3::operator[](int i) const
{
    return v[i];
}

std::ostream &operator<<(std::ostream &lhs, const vec3 &rhs)
{
    lhs << "(" << rhs[0] << "," << rhs[1] << "," << rhs[2] << ")";
    return lhs;
}

vec3 min(const vec3 &a, const vec3 &b)
{
    return vec3(std::min(a[0], b[0]), std::min(a[1], b[1]), std::min(a[2], b[2]));
}

vec3 max(const vec3 &a, const vec3 &b)
{
    return vec3(std::max(a[0], b[0]), std::max(a[1], b[1]), std::max(a[2], b[2]));
}

vec3 vec3::pow(float q) const
{
    return vec3(std::pow(v[0], q), std::pow(v[1], q), std::pow(v[2], q));
}
