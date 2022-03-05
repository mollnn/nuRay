#ifndef __MAT_GLASS_H_
#define __MAT_GLASS_H_

#include "material.h"
#include "texture.h"

class MatGlass : public Material
{
public:
    vec3 F0_;
    bool usetex_F0_;
    Texture map_F0_;
    float Ior_;

    MatGlass(const vec3& F0 = 0.0f,  float ior = 0.0f);

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
