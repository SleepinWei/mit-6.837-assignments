#include"particle.h"
#include"integrator.h"
#include"force_field.h"

void EulerIntegrator::Update(Particle *particle, ForceField *forcefield, float t, float dt){
    Vec3f position = particle->getPosition();
    float mass = particle->getMass();

    Vec3f a = forcefield->getAcceleration(position, mass, t);
    Vec3f newPos = position + particle->getVelocity() * dt;
    Vec3f newVelocity = particle->getVelocity() + a * dt;

    particle->setPosition(newPos);
    particle->setVelocity(newVelocity);

    particle->increaseAge(dt);
}

void MidpointIntegrator::Update(Particle *particle, ForceField *forcefield, float t, float dt) {
    Vec3f pn = particle->getPosition();
    Vec3f vn = particle->getVelocity();

    Vec3f pm = particle->getPosition() + particle->getVelocity() * dt * 0.5f;
    Vec3f vm = particle->getVelocity() + forcefield->getAcceleration(particle->getPosition(), particle->getMass(), t) * dt * 0.5f;

    Vec3f p_new = pn + vm * dt;
    Vec3f v_new = vn + forcefield->getAcceleration(pm, particle->getMass(), t + dt * 0.5f) * dt; 
    // age
    particle->setPosition(p_new);
    particle->setVelocity(v_new);
    particle->increaseAge(dt);
}