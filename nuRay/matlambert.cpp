#include "matlambert.h"

MatLambert::MatLambert(const vec3 &Kd) : Kd_(Kd), usetex_Kd_(false) {}

vec3 MatLambert::sampleBxdf(Sampler& sampler, const vec3 &wo, const vec3 &normal) const
{
    float r2 = sampler.random() * 0.99f;
    float phi = sampler.random() * 3.14159 * 2;
    float r = sqrt(r2);
    float h = sqrt(1 - r2);

    vec3 ax0 = abs(normal[0]) < 0.8 ? vec3(1.0f, 0.0f, 0.0f) : vec3(0.0f, 1.0f, 0.0f);
    vec3 ax1 = ax0.cross(normal).normalized();
    vec3 ax2 = normal.cross(ax1).normalized();
    vec3 wi = h * normal + r * cos(phi) * ax1 + r * sin(phi) * ax2;

    return wi;
}

vec3 MatLambert::bxdf(const vec3 &wo, const vec3 &normal, const vec3 &wi, const vec3 &uv) const
{
    if (usetex_Kd_)
    {
        return map_Kd_.pixelUV(uv[0], uv[1]) / 3.14159;
    }
    else
    {
        return Kd_ / 3.14159;
    }
}

float MatLambert::pdf(const vec3 &wo, const vec3 &normal, const vec3 &wi) const
{
    return 1.0 / 3.14159 * (wi.dot(normal) + 1e-4f);
}

bool MatLambert::isEmission() const
{
    return false;
}

bool MatLambert::isTransmission() const
{
    return false;
}

bool MatLambert::requireLightSampling(const vec3 &wo, const vec3 &normal) const
{
    return true;
}

vec3 MatLambert::emission(const vec3 &wo, const vec3 &normal) const
{
    return vec3(0.0f, 0.0f, 0.0f);
}