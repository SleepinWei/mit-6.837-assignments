#ifndef GRID_H
#define GRID_H
#include"grid.h"
#include<vector>
using std::vector; 
class Ray;
class Hit;
class Material;
class Grid;
class BoundingBox;
class Matrix;

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
#endif // GRID_H