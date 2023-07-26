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

class MarchingInfo{
public:
    float tmin;
    int i, j, k;
    float t_next_x, t_next_y, t_next_z;
    float d_tx, d_ty, d_tz;
    float sign_x, sign_y, sign_z;

    Vec3f normal; 

public:
    void nextCell();
};

class Grid: public Object3D{
public:
    vector<vector<vector<vector<Object3D*>>>> arr;
    int nx, ny, nz;

    int scale; // 可视化参数

public:
    Grid(BoundingBox *bb, int nx, int ny, int nz);
    ~Grid();

    void paint();

    virtual bool intersect(const Ray &r, Hit &h, float tmin);
    // virtual void insertIntoGrid(Grid *g, Matrix *m);

    void initializeRayMarch(MarchingInfo& mi, const Ray& r,float tmin) const; 
};
#endif // GRID_H