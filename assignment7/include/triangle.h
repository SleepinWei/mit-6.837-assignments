#ifndef TRIANGLE_H
#define TRIANGLE_H
#include"object3d.h"
#include"vectors.h"
class Grid;
class Matrix;
class Material; 

class Triangle: public Object3D{
public:
    Vec3f a;
    Vec3f b;
    Vec3f c;

    Vec3f normal; 

public:
    Triangle(Vec3f &a, Vec3f &b, Vec3f &c, Material *m);
    ~Triangle();

    virtual bool intersect(const Ray &r, Hit &h, float tmin) override;

    virtual void insertIntoGrid(Grid *g, Matrix *m);

    virtual void paint();
};

#endif // !TRIANGLE_H

