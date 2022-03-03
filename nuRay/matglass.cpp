#include "matglass.h"

MatGlass::MatGlass(const vec3 &F0, float ior) : F0_(F0), usetex_F0_(false), Ior_(ior) {}

vec3 MatGlass::sampleBxdf(Sampler& sampler, const vec3 &wo, const vec3 &normal) const
{
    if (sampler.random() < 0.5)
    {
        float I = wo.dot(normal) > 0 ? 1.0f / Ior_ : Ior_;
        vec3 n = wo.dot(normal) < 0 ? normal : -normal;
        vec3 t = n.cross(wo).cross(n).normalized();
        float sin_o = wo.dot(t);
        float sin_i = sin_o * I;
        sin_i = std::min(sin_i, 1.0f);
        float cos_i = sqrt(1.0f - sin_i * sin_i);
        vec3 i = cos_i * n + sin_i * t;
        return i;
    }
    else
    {
        vec3 n = wo.dot(normal) > 0 ? normal : -normal;
        return 2 * wo.dot(n) * n - wo;
    }
}

vec3 MatGlass::bxdf(const vec3 &wo, const vec3 &normal, const vec3 &wi, const vec3 &uv) const
{
    if (abs(wi.dot(normal)) < 1e-3f)
    {
        return 0.0f;
    }
    vec3 f0 = usetex_F0_ ? map_F0_.pixelUV(uv[0], uv[1]) : F0_;
    vec3 F = f0 + (vec3(1.0f) - f0) * pow(1 - abs(normal.dot(wo)), 5);
    if (wi.dot(normal) * wo.dot(normal) > 0)
    {
        return F;
    }
    else
    {
        float I = wo.dot(normal) > 0 ? 1.0f / Ior_ : Ior_;
        return (vec3(1.0f) - F) * pow(I, 2);
    }
}

float MatGlass::pdf(const vec3 &wo, const vec3 &normal, const vec3 &wi) const
{
    return 0.5;
}

bool MatGlass::isEmission() const
{
    return false;
}

bool MatGlass::isTransmission() const
{
    return true;
}

bool MatGlass::requireLightSampling(const vec3 &wo, const vec3 &normal) const
{
    return false;
}

vec3 MatGlass::emission(const vec3 &wo, const vec3 &normal) const
{
    return vec3(0.0f, 0.0f, 0.0f);
}