#include"matrix.h"
#include"object3d.h"

class Transform: public Object3D{
public:
    Matrix m;
    Matrix inv_m; 
    Object3D *o; 

public:
    Transform(Matrix &m, Object3D *o);
    ~Transform();

    virtual bool intersect(const Ray &r, Hit &h, float tmin) override;

    virtual void insertIntoGrid(Grid *g, Matrix *m);

    virtual void paint();
};
