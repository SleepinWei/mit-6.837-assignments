#ifndef INTEGRATOR_H 
#define INTEGRATOR_H 
#include"vectors.h"

class Particle;
class ForceField; 

class Integrator{
public:
    virtual void Update(Particle *particle, ForceField *forcefield, float t, float dt) = 0;
    virtual Vec3f getColor(){
        return {};
    }
};

class EulerIntegrator:public Integrator{
public: 
    virtual void Update(Particle *particle, ForceField *forcefield, float t, float dt);
    virtual Vec3f getColor(){
        return {1, 0, 0};
    }
};

class MidpointIntegrator: public Integrator{
public:
    virtual void Update(Particle *particle, ForceField *forcefield, float t, float dt);
    virtual Vec3f getColor(){
        return {0, 0, 1};
    }
};
#endif // !FORCE_FIELD_H