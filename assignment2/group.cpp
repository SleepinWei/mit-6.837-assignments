#include"group.h"
#include<assert.h>
#include"hit.h"

Group::Group(int number){
    instances = vector<Object3D *>(number, nullptr);
}

void Group::addObject(int index, Object3D* obj){
    assert(index < instances.size());
    assert(obj != nullptr);

    instances[index] = obj;
}

bool Group::intersect(const Ray&r, Hit& h,float tmin){
    bool result = false;
    for(auto inst: instances){
        bool isIntersect = inst->intersect(r, h, tmin);
        if(isIntersect){
            result = true;
        }
    }
    return result; 
}