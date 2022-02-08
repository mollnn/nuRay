#include "matlambert.h"

MatLambert::MatLambert(const vec3& Kd) : Kd_(Kd) {}

vec3 MatLambert::sampleBxdf(const vec3 &wo, const vec3 &normal) const
{
    float r2 = rand() * 1.0 / RAND_MAX;
    float phi = rand() * 1.0 / RAND_MAX * 3.14159 * 2;
    float r = sqrt(r2);
    float h = sqrt(1 - r2);

    vec3 ax0 = abs(normal[0]) < 0.8 ? vec3(1.0f, 0.0f, 0.0f) : vec3(0.0f, 1.0f, 0.0f);
    vec3 ax1 = ax0.cross(normal).normalized();
    vec3 ax2 = normal.cross(ax1).normalized();
    vec3 wi = h * normal + r * cos(phi) * ax1 + r * sin(phi) * ax2;

    return wi;
}

vec3 MatLambert::bxdf(const vec3 &wo, const vec3 &normal, const vec3 &wi) const
{
    return Kd_;
}

float MatLambert::pdf(const vec3 &wo, const vec3 &normal, const vec3 &wi) const
{
    return 1.0 / 3.14159;
}

bool MatLambert::isEmission() const
{
    return false;
}

bool MatLambert::requireLightSampling(const vec3 &wo, const vec3 &normal) const
{
    return true;
}

vec3 MatLambert::emission(const vec3 &wo, const vec3& normal) const
{
    return vec3(0.0f, 0.0f, 0.0f);
}