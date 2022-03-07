#ifndef __LIGHT_SAMPLER_H__
#define __LIGHT_SAMPLER_H__

#include <bits/stdc++.h>
#include "triangle.h"
#include "envmap.h"

class LightSampler 
{
    std::vector<const Triangle*> lighting_triangles_;
    std::vector<float> cdf_; // Cumulative distribution function
    std::vector<float> pdf_; // Probability distribution function
    float sum_weight_primitives_ = 0.0f;
    float sum_weight_envmap_ = 0.0f;
    const Envmap* envmap_ = nullptr;
public:
    LightSampler() = default;
    void setPrimitives(const std::vector<Triangle> &scene);
    void setEnvmap(const Envmap& envmap);
    const Triangle* sampleLight(Sampler& sampler);
    std::tuple<vec3, vec3, vec3> sampleAllLight(Sampler& sampler);
    float p();
};

#endif
