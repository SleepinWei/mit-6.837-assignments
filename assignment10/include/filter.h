#ifndef FILTER_H
#define FILTER_H
#include "vectors.h"
class Film;

class Filter
{
public:
    virtual Vec3f getColor(int i, int j, Film *film);
    virtual float getWeight(float x, float y) = 0;
    virtual int getSupportRadius() = 0;
};

class BoxFilter : public Filter
{
public:
    BoxFilter(float radius) : coeff(radius) {}
    virtual float getWeight(float x, float y);
    int getSupportRadius() { return ceil(coeff-0.5); };

public:
    float coeff; 
};

class TentFilter : public Filter
{
public:
    TentFilter(float coeff) : coeff(coeff) {}
    virtual float getWeight(float x, float y);
    int getSupportRadius() { return ceil(coeff-0.5f); }
public:
    float coeff; 
};

class GaussianFilter : public Filter
{
public:
    GaussianFilter(float coeff) : coeff(coeff) {}
    virtual float getWeight(float x, float y);
    int getSupportRadius(){return ceil(2 * coeff-0.5f); }
public:
    float coeff;
};
#endif // !1
