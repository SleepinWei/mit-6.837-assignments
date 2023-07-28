#include"object3d.h"
#include"vectors.h"
#include"matrix.h"

class Sphere: public Object3D{
public:
    Vec3f center;
    float radius;

public:
    Sphere(Vec3f center, float radius, Material *mat);

    ~Sphere();

    virtual bool intersect(const Ray &r, Hit &h, float tmin) override;

    virtual void insertIntoGrid(Grid *g, Matrix *m);

    void paint();
};