#include"GL/freeglut.h"
#include"generator.h"
#include"particle.h"

extern Random random; 

Particle* HoseGenerator::Generate(float current_time, int i){
    Vec3f v = velocity + velocity_randomness * (random.randomVector() - 0.5f);
    Vec3f pos = position + position_randomness * (random.randomVector() - 0.5f);
    Vec3f c= color + color_randomness * (random.randomVector() - 0.5f);
    float m = mass + mass_randomness * (random.next()-0.5f);
    float ls = lifespan + lifespan_randomness * (random.next()-0.5f);
    Particle *newParticle = new Particle(pos, v, c, dead_color, m,ls);
    return newParticle;
}

int RingGenerator::numNewParticles(float current_time, float dt) const
{
    float r = std::min(current_time * radius_speed, max_radius) + 1.0f;
    float num = density * 2 *  3.14f * r; 
    return num;
}

Particle* RingGenerator::Generate(float current_time, int i){
    Vec3f v = velocity + velocity_randomness * (random.randomVector() - 0.5f);
    float theta = 2 * 3.14f * random.next();
    float r = radius_speed * current_time;
    Vec3f pos = Vec3f{r*cos(theta),0.0f,r*sin(theta)} + position_randomness * (random.randomVector() - 0.5f);

    Vec3f c= color + color_randomness * (random.randomVector() - 0.5f);
    float m = mass + mass_randomness * (random.next()-0.5f);
    float ls = lifespan + lifespan_randomness * (random.next()-0.5f);

    Particle *newParticle = new Particle(pos, v, c, dead_color, m,ls);
    return newParticle;
}

// for the gui
void RingGenerator::Paint() const{
        glBegin(GL_QUADS);
        glColor3f(1, 1, 1);
        glNormal3f(0, 0, -1);
        glVertex3f(-100.0f, -1.0f, -100.0f);
        glVertex3f(-100.0f, -1.0f, 100.0f);
        glVertex3f(100.0f, -1.0f, 100.0f);
        glVertex3f(100.0f, -1.0f, -100.0f);
        glEnd();
}
// for the gui
void HoseGenerator::Paint() const{

}

void Generator::Restart(){
    random = Random(); 
}