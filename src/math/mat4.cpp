#include "../math/mat4.h"
#include <cmath>

mat4::mat4() : v{0.0f, 0.0f, 0.0f, 0.0f} {}
mat4::mat4(float x) : v{x, x, x, x} {}
mat4::mat4(const vec4 &x, const vec4 &y, const vec4 &z, const vec4 &w) : v{x, y, z, w} {}

mat4 &mat4::operator+=(const mat4 &rhs)
{
    return *this = {v[0] + rhs.v[0], v[1] + rhs.v[1], v[2] + rhs.v[2], v[3] + rhs.v[3]};
}
mat4 &mat4::operator-=(const mat4 &rhs)
{
    return *this = {v[0] - rhs.v[0], v[1] - rhs.v[1], v[2] - rhs.v[2], v[3] - rhs.v[3]};
}

mat4 &mat4::operator*=(const mat4 &rhs)
{
    mat4 lhs = T();
    mat4 ans;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            ans.m(i, j) = lhs.v[i].dot(rhs.v[j]);
        }
    }
    *this = ans;
    return *this;
}

mat4 &mat4::operator*=(float rhs)
{
    return *this = {v[0] * rhs, v[1] * rhs, v[2] * rhs, v[3] * rhs};
}
mat4 &mat4::operator/=(float rhs)
{
    return *this = {v[0] / rhs, v[1] / rhs, v[2] / rhs, v[3] / rhs};
}

mat4 mat4::operator-() const
{
    return {-v[0], -v[1], -v[2], -v[3]};
}

mat4 mat4::operator+(const mat4 &rhs) const
{
    return {v[0] + rhs.v[0], v[1] + rhs.v[1], v[2] + rhs.v[2], v[3] + rhs.v[3]};
}
mat4 mat4::operator-(const mat4 &rhs) const
{
    return {v[0] - rhs.v[0], v[1] - rhs.v[1], v[2] - rhs.v[2], v[3] - rhs.v[3]};
}

mat4 mat4::operator*(const mat4 &rhs) const
{
    mat4 ans = *this;
    ans *= rhs;
    return ans;
}

vec4 mat4::operator*(const vec4 &rhs) const
{
    mat4 lhs = T();
    vec4 ans;
    for (int i = 0; i < 4; i++)
    {
        ans[i] = lhs.v[i].dot(rhs);
    }
    return ans;
}

mat4 mat4::operator*(float rhs) const
{
    return {v[0] * rhs, v[1] * rhs, v[2] * rhs, v[3] * rhs};
}
mat4 mat4::operator/(float rhs) const
{
    return {v[0] / rhs, v[1] / rhs, v[2] / rhs, v[3] / rhs};
}

float &mat4::m(int i, int j)
{
    return v[j][i];
}

float mat4::m(int i, int j) const
{
    return v[j][i];
}

mat4 mat4::T() const
{
    mat4 ans;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            ans.m(i, j) = m(j, i);
        }
    }
    return ans;
}

vec4 operator*(const vec4 &lhs, const mat4 &rhs)
{
    vec4 ans;
    for (int i = 0; i < 4; i++)
    {
        ans[i] = lhs.dot(rhs.v[i]);
    }
    return ans;
}

vec4 &mat4::operator[](int i)
{
    return v[i];
}

vec4 mat4::operator[](int i) const
{
    return v[i];
}

std::ostream &operator<<(std::ostream &lhs, const mat4 &rhs)
{
    lhs << "(" << rhs[0] << "," << rhs[1] << "," << rhs[2] << "," << rhs[3] << ")";
    return lhs;
}
