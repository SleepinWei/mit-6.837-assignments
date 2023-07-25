#include"triangle.h"
#include"material.h"
#include"ray.h"
#include"hit.h"
#include"boundingbox.h"
#include<GL/freeglut.h>

Triangle::Triangle(Vec3f &a, Vec3f &b, Vec3f &c, Material *m){
    this->a = a;
    this->b = b;
    this->c = c;
    this->mat = m; 

    Vec3f::Cross3(normal, b - a, c - a);
    normal.Normalize();

    Vec3f pos_min;
    Vec3f pos_max;

    Vec3f::Min(pos_min, a, b);
    Vec3f::Min(pos_min, pos_min, c);

    Vec3f::Max(pos_max, a, b);
    Vec3f::Max(pos_max, pos_max, c);

    bb = new BoundingBox(pos_min, pos_max);
}

Triangle::~Triangle(){
    delete bb; 
}

bool Triangle::intersect(const Ray &r, Hit &h, float tmin) {
    Vec3f origin = r.getOrigin();
    Vec3f d = r.getDirection();

    Vec3f E1 = b - a;
    Vec3f E2 = c - a;
    Vec3f T = origin - a;
    Vec3f P;
    Vec3f::Cross3(P, d, E2);
    Vec3f Q;
    Vec3f::Cross3(Q, T, E1);

    float denominator = P.Dot3(E1);

    float t = Q.Dot3(E2) / denominator;
    float u = P.Dot3(T) / denominator;
    float v = Q.Dot3(d) / denominator;


    // decide inside or not 
    if(t < tmin || u < 0 || u >1 || v <0 || v > 1 || u+v >1){
        return false; 
    }

    if(t < h.getT()){
        Vec3f normal;
        Vec3f::Cross3(normal, E1, E2);
        normal.Normalize();
        h.set(t, this->mat, normal, r);
    }

    return true;
}

void Triangle::paint(){
    Vec3f color = this->mat->getDiffuseColor();
    Vec3f specular = this->mat->getSpecularColor();
    // glColor3f(color.x(), color.y(), color.z());
    // geometry
    mat->glSetMaterial();

    glBegin(GL_TRIANGLES);
    glNormal3f(normal.x(), normal.y(), normal.z());
    glColor3f(1.0f, 0.0f, 0.0f);


    glVertex3f(a.x(), a.y(),a.z());
    glVertex3f(b.x(), b.y(), b.z());
    glVertex3f(c.x(), c.y(), c.z());
    glEnd(); 
}