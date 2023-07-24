#include"object3d.h"
#include<vector>
using std::vector;

class Group: public Object3D{
public:
    vector<Object3D *> instances;

    virtual bool intersect(const Ray &r, Hit &h, float tmin) override;

    void addObject(int index, Object3D *obj);

public:
    Group(int number);
};