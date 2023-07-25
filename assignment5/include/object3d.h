#ifndef OBJECT_H
#define OBJECT_H
#include<vector>
using std::vector; 
class Ray;
class Hit;
class Material;
class Grid; 

class Object3D{
public:
    Object3D(){
        mat = nullptr; 
    }

    virtual ~Object3D(){

    }

    virtual bool intersect(const Ray &r, Hit &h, float tmin) = 0;

    virtual void paint() = 0; 

    virtual void insertIntoGrid(Grid *g, Matrix *m){}

    BoundingBox *getBoundingBox() { return bb; }

public:
    Material *mat;
    BoundingBox *bb; 
};

class BoundingBox; 
class Matrix; 

class Grid: public Object3D{
public:
    vector<vector<vector<bool>>> arr;
    int nx, ny, nz; 

public:
    Grid(BoundingBox *bb, int nx, int ny, int nz);

    void paint();

    virtual bool intersect(const Ray &r, Hit &h, float tmin);
    // virtual void insertIntoGrid(Grid *g, Matrix *m);
};
#endif // !OBJECT_H