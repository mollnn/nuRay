#ifndef __MATLAMBERT_H_
#define __MATLAMBERT_H_

#include "material.h"

class MatLambert : public Material
{
public:
    vec3 Kd_;

    MatLambert(const vec3& Kd);

    virtual vec3 sampleBxdf(const vec3 &wo, const vec3 &normal) const override;
    virtual vec3 bxdf(const vec3 &wo, const vec3 &normal, const vec3 &wi) const override;
    virtual float pdf(const vec3 &wo, const vec3 &normal, const vec3 &wi) const override;

    virtual bool isEmission() const override;
    virtual bool requireLightSampling(const vec3 &wo, const vec3 &normal) const override;
    virtual vec3 emission(const vec3 &wo, const vec3& normal) const override;
};

#endif
