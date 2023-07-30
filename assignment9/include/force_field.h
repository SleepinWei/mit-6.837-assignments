#ifndef FORCE_FIELD_H
#define FORCE_FIELD_H
#include"vectors.h"

class ForceField{
public:
     virtual Vec3f getAcceleration(const Vec3f &position, float mass, float t) const = 0;
}; 
class GravityForceField:public ForceField{
public:
   GravityForceField(Vec3f gravity):gravity(gravity){}
   virtual Vec3f getAcceleration(const Vec3f &position, float mass, float t) const;
   Vec3f gravity; 
};
class ConstantForceField:public ForceField{
public:
    ConstantForceField(Vec3f force):force(force){}
    virtual Vec3f getAcceleration(const Vec3f &position, float mass, float t) const;
  
    Vec3f force; 
};
class RadialForceField:public ForceField{
public: 
   RadialForceField(float magnitude):magnitude(magnitude){}
    virtual Vec3f getAcceleration(const Vec3f &position, float mass, float t) const;
  
   float magnitude;
};
class VerticalForceField:public ForceField{
public:
   VerticalForceField(float magnitude):magnitude(magnitude){}
    virtual Vec3f getAcceleration(const Vec3f &position, float mass, float t) const;
  
   float magnitude;
};
#endif // !FORCE_FIELD_H
