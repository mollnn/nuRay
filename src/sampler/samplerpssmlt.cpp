#include "../sampler/samplerpssmlt.h"

SamplerPSSMLT::SamplerPSSMLT()
{
    a.resize(100);
    ptr = 0;
    newSample();
}

float SamplerPSSMLT::random()
{
    return a[ptr++];
}

void SamplerPSSMLT::nextIter(bool large_jump, Config &config)
{
    ptr = 0;
    if (large_jump)
    {
        for (int i = 0; i < a.size(); i++)
        {
            a[i] = SamplerStd::random();
        }
    }
    else
    {
        for (int i = 0; i < a.size(); i++)
        {
            if (!configed)
            {
                cs1 = config.getValueFloat("s1", 1.0 / 1024);
                cs2 = config.getValueFloat("s2", 1.0 / 16);
                cs20 = config.getValueFloat("s20", 1.0 / 10);
                cs10 = 1.0 / config.getValueInt("imgw", 1.0);
                cs11 = 1.0 / config.getValueInt("imgh", 1.0);
                configed = true;
            }

            float s1 = cs1, s2 = cs2;
            if (i < 2)
                s2 = cs20;
            if (i == 0)
                s1 = cs10;
            if (i == 1)
                s1 = cs11;
                
            float r = SamplerStd::random();
            if (r < 0.5f)
            {
                r = 2 * r;
                a[i] += s2 * exp(-r * log(s2 / s1));
            }
            else
            {
                r = 2 * r - 1;
                a[i] -= s2 * exp(-r * log(s2 / s1));
            }
            if (a[i] <= 0.0f)
                a[i] += 1.0f;
            if (a[i] >= 1.0f)
                a[i] -= 1.0f;
        }
    }
}

void SamplerPSSMLT::newSample()
{
    ptr = 0;
    for (int i = 0; i < a.size(); i++)
    {
        a[i] = SamplerStd::random();
    }
}

void SamplerPSSMLT::repeatCurrentIter()
{
    ptr = 0;
}
