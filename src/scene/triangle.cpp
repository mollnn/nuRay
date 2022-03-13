#include "../scene/triangle.h"

Triangle::Triangle() {}

Triangle::Triangle(const vec3 &p0, const vec3 &p1, const vec3 &p2,
                   const vec3 &t0,
                   const vec3 &t1,
                   const vec3 &t2,
                   const vec3 &n0,
                   const vec3 &n1,
                   const vec3 &n2,
                   const Material *mat) : p{p0, p1, p2}, t{t0, t1, t2}, n{n0, n1, n2}, mat(mat)
{
    if (n[0].norm2() < 1e-4)
    {
        this->evalNormal();
    }
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

vec3 Triangle::getNormal(float b1, float b2) const
{
    return ((1.0f - b1 - b2) * n[0] + b1 * n[1] + b2 * n[2]).normalized();
}

vec3 Triangle::getTexCoords(float b1, float b2) const
{
    return (1.0f - b1 - b2) * t[0] + b1 * t[1] + b2 * t[2];
}

std::tuple<vec3, float, float> Triangle::sample(Sampler& sampler) const
{
    float r1 = sampler.random();
    float r2 = sampler.random();
    if (r1 + r2 > 1)
    {
        r1 = 1 - r1;
        r2 = 1 - r2;
    }
    return {(1 - r1 - r2) * p[0] + r1 * p[1] + r2 * p[2], r1, r2};
}

float Triangle::area() const
{
    return (p[1] - p[0]).cross(p[2] - p[0]).norm();
}

std::ostream &operator<<(std::ostream &lhs, const Triangle &rhs)
{
    lhs << std::fixed << std::setprecision(2) << "{" << rhs.p[0] << ", " << rhs.p[1] << ", " << rhs.p[2] << " | " << rhs.n[0] << ", " << rhs.n[1] << ", " << rhs.n[2] << "}";
    return lhs;
}

vec3 Triangle::pMin() const
{
    return min(p[0], min(p[1], p[2]));
}

vec3 Triangle::pMax() const
{
    return max(p[0], max(p[1], p[2]));
}