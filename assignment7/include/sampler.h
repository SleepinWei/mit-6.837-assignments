#ifndef SAMPLER_H
#define SAMPLER_H

#include "vectors.h"

class Sampler
{
public:
    virtual Vec2f getSamplePosition(int n) = 0;
};

class RandomSampler: public Sampler{
public:
    RandomSampler(){};
    virtual Vec2f getSamplePosition(int n) override;
};

class UniformSampler: public Sampler{
public:
    UniformSampler(){};
    virtual Vec2f getSamplePosition(int n) override;
};

class JitteredSampler: public Sampler{
public:
    JitteredSampler(){};
    virtual Vec2f getSamplePosition(int n) override;
};


#endif // !SAMPLER_H