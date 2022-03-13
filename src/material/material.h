#ifndef __MATERIAL_H_
#define __MATERIAL_H_

#include "../math/vec3.h"
#include "../sampler/sampler.h"

class Material
{
public:
    virtual vec3 sampleBxdf(Sampler &sampler, const vec3 &wo, const vec3 &normal) const = 0;
    virtual vec3 bxdf(const vec3 &wo, const vec3 &normal, const vec3 &wi, const vec3 &uv) const = 0;
    virtual float pdf(const vec3 &wo, const vec3 &normal, const vec3 &wi) const = 0;
    virtual bool isEmission() const = 0;
    virtual bool isTransmission() const = 0;
    virtual bool isSpecular(const vec3 &wo, const vec3 &normal, const vec3 &wi, const vec3 &uv) const = 0;
    virtual bool requireLightSampling(const vec3 &wo, const vec3 &normal) const = 0;
    virtual vec3 emission(const vec3 &wo, const vec3 &normal) const = 0;

    virtual float roughness(const vec3 &uv) const { return 0.0f; }
    virtual vec3 reflectanceDiffuse(const vec3 &uv) const { return 0.0f; }
    virtual vec3 reflectanceSpecular(const vec3 &uv) const { return 0.0f; }

    virtual ~Material() {}
};

#endif