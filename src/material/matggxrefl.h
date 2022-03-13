#ifndef __MAT_GGX_REFL_H_
#define __MAT_GGX_REFL_H_

#include "../material/material.h"
#include "../scene/texture.h"

class MatGGXRefl : public Material
{
public:
    // Fr0
    vec3 Kd_;
    bool usetex_Kd_;
    Texture map_Kd_;
    float alpha_;

    MatGGXRefl(const vec3& Kd = 0.0f, float alpha = 0.5f);

    virtual vec3 sampleBxdf(Sampler& sampler, const vec3 &wo, const vec3 &normal) const override;
    virtual vec3 bxdf(const vec3 &wo, const vec3 &normal, const vec3 &wi, const vec3 &uv) const override;
    virtual float pdf(const vec3 &wo, const vec3 &normal, const vec3 &wi) const override;

    virtual bool isEmission() const override;
    virtual bool isTransmission() const override;
    virtual bool isSpecular(const vec3 &wo, const vec3 &normal, const vec3 &wi, const vec3 &uv) const override;
    virtual bool requireLightSampling(const vec3 &wo, const vec3 &normal) const override;
    virtual vec3 emission(const vec3 &wo, const vec3& normal) const override;
};

#endif
