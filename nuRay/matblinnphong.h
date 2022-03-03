#ifndef __MATBLINNPHONG_H_
#define __MATBLINNPHONG_H_

#include "material.h"
#include "texture.h"

class MatBlinnPhong : public Material
{
public:
    vec3 Kd_;
    bool usetex_Kd_;
    Texture map_Kd_;

    vec3 Ks_;
    float Ns_;
    bool usetex_Ks_;
    Texture map_Ks_;

    MatBlinnPhong(const vec3& Kd = 0.0f, const vec3& Ks = 0.0f, float Ns = 10.0f);

    virtual vec3 sampleBxdf(Sampler& sampler, const vec3 &wo, const vec3 &normal) const override;
    virtual vec3 bxdf(const vec3 &wo, const vec3 &normal, const vec3 &wi, const vec3 &uv) const override;
    virtual float pdf(const vec3 &wo, const vec3 &normal, const vec3 &wi) const override;

    virtual bool isEmission() const override;
    virtual bool isTransmission() const override;
    virtual bool requireLightSampling(const vec3 &wo, const vec3 &normal) const override;
    virtual vec3 emission(const vec3 &wo, const vec3& normal) const override;
};

#endif
