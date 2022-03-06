#include "envmap.h"

Envmap::Envmap(const Texture *envmap, float gain) : envmap_(envmap), gain_(gain)
{
}

void Envmap::setEnvmap(const Texture *envmap)
{
    envmap_ = envmap;
}

void Envmap::setGain(float gain)
{
    gain_ = gain;
}

vec3 Envmap::pixelUV(float x, float y) const
{
    if (envmap_ != nullptr)
    {
        return envmap_->pixelUV(x, y);
    }
    return 0.0f;
}