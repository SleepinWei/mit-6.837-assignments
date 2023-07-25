#include"vectors.h"

class SceneParser;
class Hit;
class Ray; 

class RayTracer{
public:
    SceneParser *parser;
    int max_bounces;
    float cutoff_weight;
    bool shadows; 

public:
    RayTracer(SceneParser *s, int max_bounces, float cutoff_weight, bool shadows);

    Vec3f traceRay(Ray &ray, float tmin, int bounces, float weight, 
                 float indexOfRefraction, Hit &hit) const;

    void traceSingleRay(Ray &ray, float tmin, int bounces, float weight, 
                float indexOfRefraction, Hit &hit) const;
};