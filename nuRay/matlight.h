#ifndef __MATLIGHT_H_
#define __MATLIGHT_H_

#include "material.h"

class MatLight : public Material
{
public:
    vec3 Ke_;

    MatLight(const vec3& Ke = 0.0f);

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
