#ifndef ENVMAP_H
#define ENVMAP_H

#include "texture.h"

class Envmap
{
    const Texture *envmap_;
    float gain_;

public:
    Envmap(const Texture *envmap = nullptr, float gain = 1.0f);
    void setEnvmap(const Texture *envmap);
    void setGain(float gain);
    vec3 pixelUV(float x, float y) const;
};

#endif // ENVMAP_H
