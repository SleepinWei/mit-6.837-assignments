#ifndef RAY_TRACER_H
#define RAY_TRACER_H
#include"vectors.h"

class SceneParser;
class Hit;
class Ray;
class Grid;
class Group;

class RayTracer{
public:
    SceneParser *parser;
    int max_bounces;
    float cutoff_weight;
    bool shadows;
    Grid *grid; 

public:
    RayTracer(SceneParser *s, int max_bounces, float cutoff_weight, bool shadows);
    ~RayTracer(); 

    Vec3f traceRay(Ray &ray, float tmin, int bounces, float weight, 
                 float indexOfRefraction, Hit &hit) const;

    void traceSingleRay(Ray &ray, float tmin, int bounces, float weight, 
                float indexOfRefraction, Hit &hit) const;

private:
    bool RayCast(Ray &ray, Hit &hit, float tmin,Group* group,bool shadowRay)const;
    bool RayCastFast(Ray &ray, Hit &hit, float tmin,Group* group)const;
};

#endif // !RAY_TRACER_H