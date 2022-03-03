#include "lightsampler.h"

void LightSampler::initialize(const std::vector<Triangle> &scene)
{
    lighting_triangles_.clear();
    cdf_.clear();
    pdf_.clear();

    // Policy of probability distribution: Area currently

    float sum_weight = 0;
    for (const auto &triangle : scene)
    {
        if (triangle.mat->isEmission())
        {
            float weight = triangle.area();
            pdf_.push_back(weight);
            sum_weight += weight;
            lighting_triangles_.push_back(&triangle);
        }
    }
    sum_weight += 1e-8;
    for (auto &x : pdf_)
        x /= sum_weight;
    sum_weight_ = sum_weight;

    float sum_prob = 1e-6;
    for (int i = 0; i < pdf_.size(); i++)
    {
        sum_prob += pdf_[i];
        cdf_.push_back(sum_prob);
    }

    if (lighting_triangles_.size() == 0)
    {
        std::cerr << "LightSampler::initialize(...) called but no light" << std::endl;
    }
}

const Triangle *LightSampler::sampleLight(Sampler& sampler)
{
    if (lighting_triangles_.size() == 0)
    {
        return nullptr;
    }
    float r = sampler.random();
    int id = lower_bound(cdf_.begin(), cdf_.end(), r) - cdf_.begin();
    return lighting_triangles_[id];
}

float LightSampler::p()
{
    return 1.0 / (sum_weight_ + 1e-8);
}
