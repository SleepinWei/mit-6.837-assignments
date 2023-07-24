#include"camera.h"
#include"ray.h"

OrthographicCamera::OrthographicCamera(Vec3f center, Vec3f direction, Vec3f up, float image_size){
    this->center = center;

    float len2 = direction.Length();
    direction.Divide(len2, len2, len2);
    this->direction = direction; 

    float len1 = up.Length();
    up.Divide(len1, len1, len1);

    Vec3f::Cross3(this->horizontal, direction, up);
    this->horizontal /= this->horizontal.Length();

    Vec3f::Cross3(this->up, this->horizontal, direction);
    this->up /= this->up.Length();

    this->image_size = image_size;
}

float OrthographicCamera::getTMin() const{
    return 1e6; 
}

Ray OrthographicCamera::generateRay(Vec2f point){
    Vec3f origin = center;
    point *= 2; 
    Vec2f::Sub(point, point, {1.0f, 1.0f}); // -1 ~ 1

    origin += 0.5 * point.x() * horizontal * image_size + point.y() * up * 0.5 * image_size;

    return Ray(origin, this->direction);
}