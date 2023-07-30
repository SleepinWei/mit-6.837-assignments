#include<random>
#include"sampler.h"
Vec2f RandomSampler::getSamplePosition(int n){
    float x = rand() * 1.0f / RAND_MAX;
    float y = rand() * 1.0f / RAND_MAX;

    return {x, y};
}

#include<iostream>
Vec2f UniformSampler::getSamplePosition(int n){
    int m = sqrt(num_samples);
    int row = n / m;
    int col = n % m;

    float y = 1.0f / m * row + 0.5f / m;
    float x = 1.0f / m * col+ 0.5f / m;
    return {x, y};
}

Vec2f JitteredSampler::getSamplePosition(int n){
    int m = sqrt(num_samples);
    int row = n / m;
    int col = n % m;

    float y = 1.0f / m * row + 0.5f / m;
    float x = 1.0f / m * col+ 0.5f / m;

    x += (rand() * 1.0f / RAND_MAX - 0.5f) / m ;
    y += (rand() * 1.0f / RAND_MAX - 0.5f) / m;

    return {x, y};
}