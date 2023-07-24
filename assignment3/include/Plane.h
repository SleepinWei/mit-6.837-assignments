#ifndef PLANE_H
#define PLANE_H

#include"object3d.h"
#include"vectors.h"
class Material; 

class Plane: public Object3D{
public:
    Vec3f normal;
    float d;
    Material *m; 

public:
    Plane(Vec3f &normal, float d, Material *m);

    virtual bool intersect(const Ray &r, Hit &h, float tmin) override;
};

#endif // !PLANE_H