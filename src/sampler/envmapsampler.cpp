#include "../sampler/envmapsampler.h"
#include "../sampler/samplerstd.h"

EnvmapSampler::EnvmapSampler()
{
}

void EnvmapSampler::initialize(const Envmap &envmap)
{
    envmap_ = &envmap;
    avg_value_ = 0;
    int N_SAMPLES = 10000;
    SamplerStd sampler;
    for (int i = 0; i < N_SAMPLES; i++)
    {
        avg_value_ += sampleLight(sampler).norm();
    }
    avg_value_ /= N_SAMPLES;
}

vec3 EnvmapSampler::sampleLight(Sampler &sampler)
{
    float u = sampler.random(), v = acos(sampler.random() * 2 - 1) / 3.14159;
    return envmap_->pixelUV(u, v);
}

float EnvmapSampler::weight()
{
    return avg_value_;
}
