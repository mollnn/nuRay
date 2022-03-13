#ifndef ENVMAPSAMPLER_H
#define ENVMAPSAMPLER_H

// In most algorithms, sampling envmap is not necessary
// For photon mapping, it may be :(O)

#include <bits/stdc++.h>
#include "../scene/triangle.h"
#include "../scene/envmap.h"

class EnvmapSampler
{
    const Envmap* envmap_;
    float avg_value_;
public:
    EnvmapSampler();
    void initialize(const Envmap &envmap);
    vec3 sampleLight(Sampler &sampler);
    float weight();
};

#endif // ENVMAPSAMPLER_H
