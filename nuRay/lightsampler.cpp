#include "lightsampler.h"

void LightSampler::initialize(const std::vector<Triangle> &scene)
{
    lighting_triangles.clear();
    cdf.clear();
    pdf.clear();

    // Policy of probability distribution: Area currently

    float sum_weight = 0;
    for (const auto &triangle : scene)
    {
        if (triangle.mat->isEmission())
        {
            float weight = triangle.area();
            pdf.push_back(weight);
            sum_weight += weight;
            lighting_triangles.push_back(&triangle);
        }
    }
    sum_weight += 1e-8;
    for (auto &x : pdf)
        x /= sum_weight;
    sum_weight_ = sum_weight;

    float sum_prob = 1e-6;
    for (int i = 0; i < pdf.size(); i++)
    {
        sum_prob += pdf[i];
        cdf.push_back(sum_prob);
    }

    if (lighting_triangles.size() == 0)
    {
        std::cerr << "LightSampler::initialize(...) called but no light" << std::endl;
    }
}

const Triangle *LightSampler::sampleLight()
{
    if (lighting_triangles.size() == 0)
    {
        throw("LightSampler::sampleLight() called but no light");
    }
    float r = rand() * 1.0f / RAND_MAX;
    int id = lower_bound(cdf.begin(), cdf.end(), r) - cdf.begin();
    return lighting_triangles[id];
}

float LightSampler::p()
{
    return 1.0 / (sum_weight_ + 1e-8);
}