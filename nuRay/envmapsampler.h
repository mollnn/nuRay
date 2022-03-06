#ifndef ENVMAPSAMPLER_H
#define ENVMAPSAMPLER_H

// In most algorithms, sampling envmap is not necessary
// For photon mapping, it may be :(O)

#include <bits/stdc++.h>
#include "triangle.h"
#include "envmap.h"

class EnvmapSampler
{

public:
    EnvmapSampler();
    void initialize(const Envmap &envmap);
    vec3 sampleLight(Sampler &sampler);
};

#endif // ENVMAPSAMPLER_H
