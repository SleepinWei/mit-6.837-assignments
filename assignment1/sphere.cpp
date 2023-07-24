#include"sphere.h"
#include"ray.h"
#include"hit.h"

Sphere::Sphere(Vec3f center, float radius, Material *mat){
    this->center = center;
    this->radius = radius;
    this->mat = mat; 
}

bool Sphere::intersect(const Ray &r, Hit &h, float tmin){
    Vec3f ray_dir = r.getDirection(); 
    // 假设 ray 归一化
    Vec3f R0 = (center - r.getOrigin());
    float tp = ray_dir.Dot3(R0);
    float R02 = R0.Dot3(R0);
    float d2 = R02 - tp * tp;
    float r2 = this->radius * this->radius;

    if(d2 > r2){
        // no intersection
        return false; 
    }

    float t_prime_2 = r2 - d2;
    float t_prime = sqrtf(t_prime_2);
    float t = 0; 

    //if(R02 < r2){
        // inside
        //t = tp + t_prime; 
    //}
    //else{
        t = tp - t_prime;  // only for orthographic
    //}

    //if(t < 0.0f){
        // no intersection
        //return false;
    //}

    if(tmin > t){
        h.set(t, this->mat, r);
    }
}
