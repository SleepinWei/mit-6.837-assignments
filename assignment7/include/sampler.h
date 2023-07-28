#ifndef SAMPLER_H
#define SAMPLER_H

#include "vectors.h"

class Sampler
{
public:
    Sampler(int num_samples):num_samples(num_samples){}
    int num_samples; 
public:
    virtual Vec2f getSamplePosition(int n) = 0;
};

class RandomSampler: public Sampler{
public:
    RandomSampler(int num_samples):Sampler(num_samples){};
    virtual Vec2f getSamplePosition(int n) override;
};

class UniformSampler: public Sampler{
public:
    UniformSampler(int num_samples):Sampler(num_samples){};
    virtual Vec2f getSamplePosition(int n) override;
};

class JitteredSampler: public Sampler{
public:
    JitteredSampler(int num_samples):Sampler(num_samples){};
    virtual Vec2f getSamplePosition(int n) override;
};


#endif // !SAMPLER_H