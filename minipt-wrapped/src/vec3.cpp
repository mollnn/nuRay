#include "vec3.hpp"

double x, y, z;
vec3 vec3::operator+(const vec3 &rhs) const { return {x + rhs.x, y + rhs.y, z + rhs.z}; }
vec3 vec3::operator-(const vec3 &rhs) const { return {x - rhs.x, y - rhs.y, z - rhs.z}; }
vec3 vec3::operator*(const vec3 &rhs) const { return {x * rhs.x, y * rhs.y, z * rhs.z}; }
vec3 vec3::operator*(double rhs) const { return {x * rhs, y * rhs, z * rhs}; }
vec3 vec3::operator/(double rhs) const { return {x / rhs, y / rhs, z / rhs}; }
double vec3::dot(const vec3 &rhs) const { return x * rhs.x + y * rhs.y + z * rhs.z; }
vec3 vec3::cross(const vec3 &rhs) const { return {y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x}; }
double vec3::norm2() const { return x * x + y * y + z * z; }
double vec3::norm() const { return sqrt(norm2()); }
vec3 vec3::unit() const { return (*this) / norm(); }
double vec3::avg() const { return (x + y + z) / 3; }
vec3 operator*(double lhs, const vec3 &rhs) { return {lhs * rhs.x, lhs * rhs.y, lhs * rhs.z}; }