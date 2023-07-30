#ifndef GENERATOR_H
#define GENERATOR_h
#include "random.h"
#include "vectors.h"

class Particle;
class Generator
{
public:
    void SetColors(Vec3f color, Vec3f dead_color, float color_randomness)
    {
        this->color = color;
        this->dead_color = dead_color;
        this->color_randomness = color_randomness;
    }
    void SetLifespan(float lifespan, float lifespan_randomness, int desired_num_particles)
    {
        this->lifespan = lifespan;
        this->lifespan_randomness = lifespan_randomness;
        this->num_particles = desired_num_particles;
    }
    void SetMass(float mass, float mass_randomness)
    {
        this->mass = mass;
        this->mass_randomness = mass_randomness;
    }

    virtual int numNewParticles(float current_time, float dt) const
    {
        return dt * num_particles / lifespan;
    }
    virtual Particle *Generate(float current_time, int i) = 0;

    virtual void Paint() const {};
    virtual void Restart();

public:
    Vec3f color, dead_color;
    float color_randomness;

    float lifespan, lifespan_randomness;

    int num_particles;
    float mass, mass_randomness;
};

class HoseGenerator : public Generator
{
public:
    HoseGenerator(Vec3f position, float position_randomness, Vec3f velocity, float velocity_randomness)
    {
        this->position = position;
        this->position_randomness = position_randomness;
        this->velocity = velocity;
        this->velocity_randomness = velocity_randomness;
    }

    virtual void Paint() const;
    virtual Particle *Generate(float current_time, int i);

    Vec3f position;
    float position_randomness;
    Vec3f velocity;
    float velocity_randomness;
};

class RingGenerator : public Generator
{
public:
    RingGenerator(float position_randomness, Vec3f velocity, float velocity_randomness)
    {
        this->position_randomness = position_randomness;
        this->velocity_randomness = velocity_randomness;
        this->velocity = velocity;
    }
    virtual Particle *Generate(float current_time, int i);
    virtual int numNewParticles(float current_time, float dt) const;

    void Paint() const;

    float position_randomness;
    float velocity_randomness;
    Vec3f velocity; 
    const float radius_speed = 0.4f;
    const float max_radius = 10.0f;
    const float density = 10.0f;
    const float lifespan = 1.0f; 
};
#endif // !GENERATOR_H