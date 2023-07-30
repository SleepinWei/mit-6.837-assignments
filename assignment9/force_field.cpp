#include"force_field.h"

Vec3f GravityForceField::getAcceleration(const Vec3f &position, float mass, float t) const{
    return gravity;
}

Vec3f ConstantForceField::getAcceleration(const Vec3f &position, float mass, float t) const{
    Vec3f result = force;
    result.Divide(mass, mass, mass);
    return result; 
}

Vec3f RadialForceField::getAcceleration(const Vec3f &position, float mass, float t) const{
    Vec3f dir = position;
    dir.Negate();
    dir.Normalize();
    float distance = sqrtf(position.Dot3(position));
    return magnitude * distance * dir; 
}

Vec3f VerticalForceField::getAcceleration(const Vec3f &position, float mass, float t) const{
    float dir_y = position.y() > 0 ? -1.0f:1.0f;
    Vec3f dir{0, dir_y, 0};
    return dir * magnitude * fabs(position.y());
}