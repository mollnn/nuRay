#include "samplerstd.h"

SamplerStd::SamplerStd()
{
}

float SamplerStd::random()
{
    return rand() * 1.0f / RAND_MAX;
}