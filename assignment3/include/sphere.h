#include"object3d.h"
#include"vectors.h"

class Sphere: public Object3D{
public:
    Vec3f center;
    float radius;

    int tessx;
    int tessy; 

public:
    Sphere(Vec3f center, float radius, Material *mat,int tessx=10,int tessy=10);

    virtual bool intersect(const Ray &r, Hit &h, float tmin) override;

    void paint();
};