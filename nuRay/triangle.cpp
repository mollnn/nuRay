#include "triangle.h"

Triangle::Triangle() {}

Triangle::Triangle(const vec3 &p0, const vec3 &p1, const vec3 &p2) : p{p0, p1, p2} 
{
    this->evalNormal();
}

void Triangle::evalNormal()
{
    n[0] = (p[1] - p[0]).cross(p[2] - p[0]).normalized();
    n[1] = n[0];
    n[2] = n[0];
}

std::tuple<float, float, float> Triangle::intersection(const vec3 &o, const vec3 &d) const
{
    vec3 e1 = p[1] - p[0], e2 = p[2] - p[0];
    vec3 s = o - p[0], s1 = d.cross(e2), s2 = s.cross(e1);
    float t = s2.dot(e2), b1 = s1.dot(s), b2 = s2.dot(d), q = s1.dot(e1) + 1e-6;
    return {t / q, b1 / q, b2 / q};
}

vec3 Triangle::getNormal(float b1, float b2)const
{
    return ((1.0f - b1 - b2) * n[0] + b1 * n[1] + b2 * n[2]).normalized();
}

vec3 Triangle::getTexCoords(float b1, float b2)const
{
    return (1.0f - b1 - b2) * t[0] + b1 * t[1] + b2 * t[2];
}

std::ostream &operator<<(std::ostream &lhs, const Triangle &rhs)
{
    lhs << std::fixed << std::setprecision(2) << "{" << rhs.p[0] << ", " << rhs.p[1] << ", " << rhs.p[2] << " | " << rhs.n[0] << ", " << rhs.n[1] << ", " << rhs.n[2] << "}";
    return lhs;
}