#include"Plane.h"
#include"material.h"
#include"ray.h"
#include"hit.h"

Plane::Plane(Vec3f &normal, float d, Material *m){
    this->normal = normal;
    this->d = d;
    this->m = m; 
}

bool Plane::intersect(const Ray &r, Hit &h, float tmin) {
    float dot = r.getDirection().Dot3(this->normal);
    if(fabs(dot) < 1e-6){
        // dot = 0
        return false; 
    }

    float distance =d - r.getOrigin().Dot3(this->normal);  // can be negative
    float t = distance / dot; 

    if(t < tmin){
        return false; 
    }

    if(t < h.getT()){
        h.set(t, m, normal, r);
    }
    return true; 
}