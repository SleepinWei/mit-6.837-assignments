#include"group.h"
#include<assert.h>
#include"hit.h"
#include"boundingbox.h"

Group::Group(int number){
    instances = vector<Object3D *>(number, nullptr);
    bb = nullptr; 
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

void Group::paint(){
    for(auto inst: instances){
        inst->paint();
    }
}

void Group::insertIntoGrid(Grid *g, Matrix *m){
    for(auto inst : instances){
        if(inst->getBoundingBox()!=nullptr){
            inst->insertIntoGrid(g, m);
        }
    }
}

void Group::generateBoundingBox(){
    Vec3f pos_min(INFINITY,INFINITY,INFINITY);
    Vec3f pos_max(-1e6,-1e6,-1e6); 

    for (int i = 0; i < instances.size();i++)
    {
        auto inst = instances[i];
        inst->generateBoundingBox(); 

        auto bounding_box = inst->getBoundingBox();
        if(bounding_box){
            Vec3f::Min(pos_min, pos_min, bounding_box->getMin());
            Vec3f::Max(pos_max, pos_max, bounding_box->getMax());
        }
    }

    bb = new BoundingBox(pos_min,pos_max);
}

Group::~Group(){
    if(bb){
        delete bb; 
    }
}