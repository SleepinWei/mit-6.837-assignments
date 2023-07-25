#include<GL/freeglut.h>
#include"Plane.h"
#include"material.h"
#include"ray.h"
#include"hit.h"

Plane::Plane(Vec3f &normal, float d, Material *m){
    this->normal = normal;
    this->d = d;
    this->mat = m; 
}

bool Plane::intersect(const Ray &r, Hit &h, float tmin) {
    float dot = r.getDirection().Dot3(this->normal);
    if(fabs(dot) < 1e-6){
        // dot = 0
        return false; 
    }

    float distance =d - r.getOrigin().Dot3(this->normal);  // can be negative
    float t = distance / dot; 

    if(t < tmin){
        return false; 
    }

    if(t < h.getT()){
        h.set(t, mat, normal, r);
    }
    return true; 
}

void Plane::paint(){
    Vec3f color = this->mat->getDiffuseColor();
    this->mat->glSetMaterial();

    const int size = 1e4;
    Vec3f x_axis(1.0f, 0.0f, 0.0f);
    Vec3f x;
    Vec3f z;
    Vec3f::Cross3(z, x_axis, normal);
    Vec3f::Cross3(x, normal, z);
    z.Normalize();
    x.Normalize();

    Vec3f center = d * normal;
    Vec3f pos1 = center - size * x - size * z;
    Vec3f pos2 = center - size * x + size * z;
    Vec3f pos3 = center + size * x + size * z;
    Vec3f pos4 = center + size * x - size * z; 

    glBegin(GL_QUADS);
    glNormal3f(normal.x(), normal.y(), normal.z());
    glVertex3f(pos1.x(), pos1.y(), pos1.z());
    glVertex3f(pos2.x(), pos2.y(), pos2.z());
    glVertex3f(pos3.x(), pos3.y(), pos3.z());
    glVertex3f(pos4.x(), pos4.y(), pos4.z());
    glEnd();
}