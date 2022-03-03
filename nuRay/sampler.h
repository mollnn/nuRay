#ifndef SAMPLER_H
#define SAMPLER_H

class Sampler
{
public:
    Sampler();
    virtual float random() = 0;
};

#endif // SAMPLER_H
