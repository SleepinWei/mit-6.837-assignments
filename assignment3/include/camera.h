#include"vectors.h"
class Ray; 

class Camera{
public:
    virtual Ray generateRay(Vec2f point) = 0;
    virtual float getTMin() const = 0; 
};

class OrthographicCamera : public Camera{
public:
    Vec3f center;
    Vec3f horizontal;
    Vec3f up;
    Vec3f direction;
    float image_size;

public:
    OrthographicCamera(Vec3f center, Vec3f direction, Vec3f up, float image_size);
    virtual float getTMin() const override;
    virtual Ray generateRay(Vec2f point);

};

class PerspectiveCamera: public Camera{
public:
    Vec3f center;
    Vec3f direction;
    Vec3f up;
    float angle;
    Vec3f horizontal; 

public:
    PerspectiveCamera(Vec3f &center, Vec3f &direction, Vec3f &up, float angle);
    virtual float getTMin() const override;
    virtual Ray generateRay(Vec2f point);
};