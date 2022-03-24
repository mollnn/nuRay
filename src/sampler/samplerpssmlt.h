#ifndef SAMPLERPSSMLT_H
#define SAMPLERPSSMLT_H

#include "../sampler/samplerstd.h"
#include "../utils/config.h"
#include <bits/stdc++.h>

class SamplerPSSMLT: public SamplerStd
{
    std::vector<float> a;
    int ptr;
public:
    SamplerPSSMLT();
    virtual float random() override;
    void nextIter(bool large_jump, Config& config);
    void newSample();
    void repeatCurrentIter();
};

#endif // SAMPLERPSSMLT_H
