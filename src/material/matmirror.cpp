#include "../material/matmirror.h"

MatMirror::MatMirror(const vec3 &F0, float ior) : F0_(F0), usetex_F0_(false), Ior_(ior) {}

vec3 MatMirror::sampleBxdf(Sampler &sampler, const vec3 &wo, const vec3 &normal) const
{
    vec3 n = wo.dot(normal) > 0 ? normal : -normal;
    return 2 * wo.dot(n) * n - wo;
}

std::tuple<vec3, vec3, float> MatMirror::sampleBxdfNew(Sampler &sampler, const vec3 &wo, const vec3 &normal) const
{
    return {sampleBxdf(sampler, wo, normal), F0_, 1.0f};
}

vec3 MatMirror::bxdf(const vec3 &wo, const vec3 &normal, const vec3 &wi, const vec3 &uv) const
{
    return 0.0f;
}

float MatMirror::pdf(const vec3 &wo, const vec3 &normal, const vec3 &wi) const
{
    return 1e-18f;
}

bool MatMirror::isEmission() const
{
    return false;
}

bool MatMirror::isTransmission() const
{
    return false;
}

bool MatMirror::isSpecular(const vec3 &wo, const vec3 &normal, const vec3 &wi, const vec3 &uv) const
{
    return true;
}

bool MatMirror::requireLightSampling(const vec3 &wo, const vec3 &normal) const
{
    return false;
}

vec3 MatMirror::emission(const vec3 &wo, const vec3 &normal) const
{
    return vec3(0.0f, 0.0f, 0.0f);
}