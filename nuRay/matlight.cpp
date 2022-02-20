#include "MatLight.h"

MatLight::MatLight(const vec3& Ke) : Ke_(Ke) {}

vec3 MatLight::sampleBxdf(const vec3 &wo, const vec3 &normal) const
{
    return vec3(0.0f, 0.0f, 0.0f);
}

vec3 MatLight::bxdf(const vec3 &wo, const vec3 &normal, const vec3 &wi, const vec3 &uv) const
{
    return vec3(0.0f, 0.0f, 0.0f);
}

float MatLight::pdf(const vec3 &wo, const vec3 &normal, const vec3 &wi) const
{
    return 1.0;
}

bool MatLight::isEmission() const
{
    return true;
}

bool MatLight::isTransmission() const
{
    return true;
}

bool MatLight::requireLightSampling(const vec3 &wo, const vec3 &normal) const
{
    return false;
}

vec3 MatLight::emission(const vec3 &wo, const vec3& normal) const
{
    return Ke_;
}