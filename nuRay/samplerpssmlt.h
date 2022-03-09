#ifndef SAMPLERPSSMLT_H
#define SAMPLERPSSMLT_H

#include "samplerstd.h"
#include <bits/stdc++.h>

class SamplerPSSMLT: public SamplerStd
{
    std::vector<float> a;
    int ptr;
public:
    SamplerPSSMLT();
    virtual float random() override;
    void nextIter(bool large_jump);
    void newSample();
    void repeatCurrentIter();
};

#endif // SAMPLERPSSMLT_H
