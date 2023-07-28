#include<random>
#include"sampler.h"
Vec2f RandomSampler::getSamplePosition(int n){
    float x = rand() * 1.0f / RAND_MAX;
    float y = rand() * 1.0f / RAND_MAX;

    return {x, y};
}

Vec2f UniformSampler::getSamplePosition(int n){

    
    return {x, y};
}

Vec2f JitteredSampler::getSamplePosition(int n){
    float x = rand() * 1.0f / RAND_MAX;
    float y = rand() * 1.0f / RAND_MAX;

    return {x, y};
}