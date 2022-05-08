#ifndef __MAT_Mirror_H_
#define __MAT_Mirror_H_

#include "../material/material.h"
#include "../scene/texture.h"

class MatMirror : public Material
{
public:
    vec3 F0_;
    bool usetex_F0_;
    Texture map_F0_;
    float Ior_; // todo delete ior

    MatMirror(const vec3 &F0 = 0.0f, float ior = 0.0f);

    // virtual std::tuple<vec3, vec3, float> sampleBxdfNew(Sampler &sampler, const vec3 &wo, const vec3 &normal) const override;
    virtual vec3 sampleBxdf(Sampler &sampler, const vec3 &wo, const vec3 &normal) const override;
    virtual vec3 bxdf(const vec3 &wo, const vec3 &normal, const vec3 &wi, const vec3 &uv) const override;
    virtual float pdf(const vec3 &wo, const vec3 &normal, const vec3 &wi) const override;

    virtual bool isEmission() const override;
    virtual bool isTransmission() const override;
    virtual bool isSpecular(const vec3 &wo, const vec3 &normal, const vec3 &wi, const vec3 &uv) const override;
    virtual bool requireLightSampling(const vec3 &wo, const vec3 &normal) const override;
    virtual vec3 emission(const vec3 &wo, const vec3 &normal) const override;
};

#endif
