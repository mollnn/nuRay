#include "../math/vec4.h"
#include <cmath>

vec4::vec4() : v{0.0f, 0.0f, 0.0f, 0.0f} {}
vec4::vec4(float x) : v{x, x, x, x} {}
vec4::vec4(float x, float y, float z, float w) : v{x, y, z, w} {}
vec4::vec4(const vec3 &x, float w) : v{x[0], x[1], x[2], w} {}
vec3 vec4::xyz() const { return {v[0], v[1], v[2]}; }

vec4 &vec4::operator+=(const vec4 &rhs)
{
    return *this = {v[0] + rhs.v[0], v[1] + rhs.v[1], v[2] + rhs.v[2], v[3] + rhs.v[3]};
}
vec4 &vec4::operator-=(const vec4 &rhs)
{
    return *this = {v[0] - rhs.v[0], v[1] - rhs.v[1], v[2] - rhs.v[2], v[3] - rhs.v[3]};
}
vec4 &vec4::operator*=(const vec4 &rhs)
{
    return *this = {v[0] * rhs.v[0], v[1] * rhs.v[1], v[2] * rhs.v[2], v[3] * rhs.v[3]};
}
vec4 &vec4::operator*=(float rhs)
{
    return *this = {v[0] * rhs, v[1] * rhs, v[2] * rhs, v[3] * rhs};
}
vec4 &vec4::operator/=(float rhs)
{
    return *this = {v[0] / rhs, v[1] / rhs, v[2] / rhs, v[3] / rhs};
}

vec4 vec4::operator-() const
{
    return {-v[0], -v[1], -v[2], -v[3]};
}

vec4 vec4::operator+(const vec4 &rhs) const
{
    return {v[0] + rhs.v[0], v[1] + rhs.v[1], v[2] + rhs.v[2], v[3] + rhs.v[3]};
}
vec4 vec4::operator-(const vec4 &rhs) const
{
    return {v[0] - rhs.v[0], v[1] - rhs.v[1], v[2] - rhs.v[2], v[3] - rhs.v[3]};
}
vec4 vec4::operator*(const vec4 &rhs) const
{
    return {v[0] * rhs.v[0], v[1] * rhs.v[1], v[2] * rhs.v[2], v[3] * rhs.v[3]};
}
vec4 vec4::operator*(float rhs) const
{
    return {v[0] * rhs, v[1] * rhs, v[2] * rhs, v[3] * rhs};
}
vec4 vec4::operator/(float rhs) const
{
    return {v[0] / rhs, v[1] / rhs, v[2] / rhs, v[3] / rhs};
}
float vec4::dot(const vec4 &rhs) const
{
    return v[0] * rhs.v[0] + v[1] * rhs.v[1] + v[2] * rhs.v[2] + v[3] * rhs.v[3];
    ;
}

float vec4::norm2() const
{
    return v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3];
}
float vec4::norm() const
{
    return sqrt(norm2());
}
vec4 vec4::normalized() const
{
    return (*this) / (norm() + 1e-8f);
}
float vec4::avg() const
{
    return (v[0] + v[1] + v[2] + v[3]) / 4;
}
vec4 operator*(float lhs, const vec4 &rhs)
{
    return rhs * lhs;
}

float &vec4::operator[](int i)
{
    return v[i];
}

float vec4::operator[](int i) const
{
    return v[i];
}

std::ostream &operator<<(std::ostream &lhs, const vec4 &rhs)
{
    lhs << "(" << rhs[0] << "," << rhs[1] << "," << rhs[2] << "," << rhs[3] << ")";
    return lhs;
}

vec4 min(const vec4 &a, const vec4 &b)
{
    return vec4(std::min(a[0], b[0]), std::min(a[1], b[1]), std::min(a[2], b[2]), std::min(a[3], b[3]));
}

vec4 max(const vec4 &a, const vec4 &b)
{
    return vec4(std::max(a[0], b[0]), std::max(a[1], b[1]), std::max(a[2], b[2]), std::max(a[3], b[3]));
}

vec4 vec4::pow(float q) const
{
    return vec4(std::pow(v[0], q), std::pow(v[1], q), std::pow(v[2], q), std::pow(v[3], q));
}
