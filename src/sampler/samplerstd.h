#ifndef SAMPLERSTD_H
#define SAMPLERSTD_H

#include <bits/stdc++.h>
#include "../sampler/sampler.h"

class SamplerStd : public Sampler
{
public:
    SamplerStd();
    virtual float random() override;
};

#endif // SAMPLERSTD_H
