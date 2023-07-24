#include"vectors.h"
#include <GL/freeglut.h>
class Ray; 

class Camera{
public:
    virtual Ray generateRay(Vec2f point) = 0;
    virtual float getTMin() const = 0; 

    virtual void glInit(int w, int h) = 0;
    virtual void glPlaceCamera(void) = 0;
    virtual void dollyCamera(float dist) = 0;
    virtual void truckCamera(float dx, float dy) = 0;
    virtual void rotateCamera(float rx, float ry) = 0;
};

class OrthographicCamera : public Camera{
public:
    Vec3f center;
    Vec3f up;
    Vec3f direction;
    float image_size;

public:
    OrthographicCamera(Vec3f center, Vec3f direction, Vec3f up, float image_size);
    virtual float getTMin() const override;
    virtual Ray generateRay(Vec2f point);

    virtual void glInit(int w, int h) override;
    virtual void glPlaceCamera(void) override;
    virtual void dollyCamera(float dist) override;
    virtual void truckCamera(float dx, float dy) override;
    virtual void rotateCamera(float rx, float ry) override;
};

class PerspectiveCamera: public Camera{
public:
    Vec3f center;
    Vec3f direction;
    Vec3f up;
    float angle;

public:
    PerspectiveCamera(Vec3f &center, Vec3f &direction, Vec3f &up, float angle);
    virtual float getTMin() const override;
    virtual Ray generateRay(Vec2f point);

    virtual void glInit(int w, int h) override;
    virtual void glPlaceCamera(void) override;
    virtual void dollyCamera(float dist) override;
    virtual void truckCamera(float dx, float dy) override;
    virtual void rotateCamera(float rx, float ry) override;
};