#include<GL/freeglut.h>
#include"material.h"
#include"sphere.h"
#include"ray.h"
#include"hit.h"

Sphere::Sphere(Vec3f center, float radius, Material *mat,int tessx,int tessy){
    this->center = center;
    this->radius = radius;
    this->mat = mat;
    this->tessx = tessx;
    this->tessy = tessy;
}

bool Sphere::intersect(const Ray &r, Hit &h, float tmin){
    Vec3f ray_dir = r.getDirection(); 
    // 假设 ray 归一化
    Vec3f R0 = (center - r.getOrigin());
    float tp = ray_dir.Dot3(R0);
    float R02 = R0.Dot3(R0);
    float d2 = R02 - tp * tp;
    float r2 = this->radius * this->radius;

    if(d2 > r2){
        // no intersection
        return false; 
    }

    float t_prime_2 = r2 - d2;
    float t_prime = sqrtf(t_prime_2);
    float t = 0; 

    //if(R02 < r2){
        // inside
        //t = tp + t_prime; 
    //}
    //else{
    t = tp - t_prime;  // only for orthographic
    //}

    if(t < tmin){
        // no intersection
        t = tp + t_prime; 
        if(t < tmin)
            return false;
    }

    if(t < h.getT()){
        Vec3f hit_normal = r.pointAtParameter(t) - center;
        hit_normal.Normalize();
        h.set(t, this->mat, hit_normal,r);
    }

    return true;
}

Vec3f getPositionOnSphere(float theta_x, float theta_y){
    Vec3f pos(cos(theta_y),sin(theta_y) * cos(theta_x),sin(theta_y) * sin(theta_x));
    return pos;
}

void Sphere::paint(){
    const float pi = 3.14f;

    Vec3f color = this->mat->getDiffuseColor();

    glBegin(GL_QUADS);
    glColor3f(color.x(), color.y(), color.z());
    for (int i = 0; i < tessy; ++i){
        for (int j = 0; j < tessx;++j){
            float theta_y = pi / (tessy) * i - 0.5f ; 
            float theta_x = pi / tessx * j;

            Vec3f pos1 = getPositionOnSphere(theta_x, theta_y);
            Vec3f pos2 = getPositionOnSphere(theta_x, theta_y + pi / tessy);
            Vec3f pos3 = getPositionOnSphere(theta_x + 2 * pi / tessx, theta_y + pi / tessy);
            Vec3f pos4 = getPositionOnSphere(theta_x + 2 * pi / tessx, theta_y);

            glNormal3f(pos1.x(),pos1.y(),pos1.z());
            pos1 *= radius; 
            glVertex3f(pos1.x(),pos1.y(),pos1.z());

            glNormal3f(pos2.x(),pos2.y(),pos2.z());
            pos2 *= radius;
            glVertex3f(pos2.x(),pos2.y(),pos2.z());

            glNormal3f(pos3.x(),pos3.y(),pos3.z());
            pos3 *= radius;
            glVertex3f(pos3.x(),pos3.y(),pos3.z());

            glNormal3f(pos4.x(),pos4.y(),pos4.z());
            pos4 *= radius; 
            glVertex3f(pos4.x(),pos4.y(),pos4.z());
        }
    }
    glEnd();
}