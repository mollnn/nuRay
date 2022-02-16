#ifndef __LIGHT_SAMPLER_H__
#define __LIGHT_SAMPLER_H__

#include <bits/stdc++.h>
#include "triangle.h"

class LightSampler 
{
    std::vector<const Triangle*> lighting_triangles_;
    std::vector<float> cdf_; // Cumulative distribution function
    std::vector<float> pdf_; // Probability distribution function
    float sum_weight_;

public:
    LightSampler() = default;
    void initialize(const std::vector<Triangle> &scene);
    const Triangle* sampleLight();
    float p();
};

#endif
