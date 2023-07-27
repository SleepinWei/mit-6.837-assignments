#ifndef GRID_H
#define GRID_H
#include<vector>
using std::vector; 
#include"object3d.h"
#include"vectors.h"
class Ray;
class Hit;
class Material;
class Grid;
class BoundingBox;
class Matrix;
class Object3D;

class BoundingBox; 
class Matrix;
class Transform;

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
    vector<vector<vector<vector<Transform*>>>> arr;
    vector<Object3D *> other_objects; 
    int nx, ny, nz;
    MarchingInfo info; 

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