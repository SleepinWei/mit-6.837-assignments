#ifndef OBJECT_H
#define OBJECT_H
class Ray;
class Hit;
class Material; 

class Object3D{
public:
    Object3D(){
        mat = nullptr; 
    }

    virtual ~Object3D(){

    }

    virtual bool intersect(const Ray &r, Hit &h, float tmin) = 0;

public:
    Material *mat; 
};
#endif // !OBJECT_H