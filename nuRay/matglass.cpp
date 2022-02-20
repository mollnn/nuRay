#include "matglass.h"

MatGlass::MatGlass(const vec3 &Kt, float It) : Kt_(Kt), usetex_Kt_(false), It_(It) {}

vec3 MatGlass::sampleBxdf(const vec3 &wo, const vec3 &normal) const
{
    // TODO: Fresnel Required
    float I = wo.dot(normal) > 0 ? 1.0f / It_ : It_;
    vec3 n = wo.dot(normal) > 0 ? normal : -normal;
    vec3 t = n.cross(wo).cross(n).normalized();
    float sin_o = wo.dot(t);
    float sin_i = sin_o * I;
    sin_i = std::min(sin_i, 1.0f);
    float cos_i = sqrt(1.0f - sin_i * sin_i);
    vec3 i = cos_i * n + sin_i * t;
    return -i;
}

vec3 MatGlass::bxdf(const vec3 &wo, const vec3 &normal, const vec3 &wi, const vec3 &uv) const
{
    if(abs(wi.dot(normal)) < 1e-3f) 
    {
        return 0.0f;
    }
    if (usetex_Kt_)
    {
        return map_Kt_.pixelUV(uv[0], uv[1]);
    }
    else
    {
        return Kt_;
    }
}

float MatGlass::pdf(const vec3 &wo, const vec3 &normal, const vec3 &wi) const
{
    return 1.0; 
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