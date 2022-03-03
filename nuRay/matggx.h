#ifndef __MAT_GGX_H_
#define __MAT_GGX_H_

#include "material.h"
#include "texture.h"

class MatGGX : public Material
{
public:
    // Fr0
    vec3 Kd_;
    bool usetex_Kd_;
    Texture map_Kd_;
    float alpha_;
    float ior_; // ! RELATIVE to simplify

    MatGGX(const vec3 &Kd = 0.0f, float alpha = 0.5f, float ior = 1.0f);

    virtual vec3 sampleBxdf(Sampler& sampler, const vec3 &wo, const vec3 &normal) const override;
    virtual vec3 bxdf(const vec3 &wo, const vec3 &normal, const vec3 &wi, const vec3 &uv) const override;
    virtual float pdf(const vec3 &wo, const vec3 &normal, const vec3 &wi) const override;

    virtual bool isEmission() const override;
    virtual bool isTransmission() const override;
    virtual bool requireLightSampling(const vec3 &wo, const vec3 &normal) const override;
    virtual vec3 emission(const vec3 &wo, const vec3 &normal) const override;

private:
    vec3 F(const vec3 &v, const vec3 &m, const vec3 &Kd) const;
    float G1(const vec3 &v, const vec3 &m, const vec3 &n) const;
    float G(const vec3 &i, const vec3 &o, const vec3 &m, const vec3 &n) const;
    float D(const vec3 &m, const vec3 &n) const;
    vec3 fr(const vec3 &i, const vec3 &o, const vec3 &n, const vec3 &Kd) const;
    vec3 ft(const vec3 &i, const vec3 &o, const vec3 &n, const vec3 &Kd) const;
};

#endif
