#include"hit.h"
#include"ray.h"
#include"Transform.h"
#include"matrix.h"
#include<GL/freeglut.h>

Transform::Transform(Matrix& m,Object3D* o){
    this->m = m;
    this->o = o;
    this->inv_m = m;
    int success = this->inv_m.Inverse();
}

bool Transform::intersect(const Ray &r, Hit &h, float tmin) {
    Vec3f origin = r.getOrigin();
    Vec3f dir = r.getDirection(); 

    inv_m.Transform(origin);
    Vec4f dir_4d(dir.x(), dir.y(), dir.z(), 0.0f);
    inv_m.Transform(dir_4d);
    Vec3f dir_3d(dir_4d.x(),dir_4d.y(),dir_4d.z());
    dir_3d.Normalize();

    // only make sense when w component of transformed result is 0.0f 
    Ray r_transformed(origin, dir_3d);

    Hit new_h(1e6,nullptr,{}); 

    bool isIntersect = o->intersect(r_transformed, new_h, tmin);

    if(!isIntersect){
        return false;
    }

    // 计算实际击中点
    Vec3f actual_hitpoint = new_h.getIntersectionPoint();
    m.Transform(actual_hitpoint);

    float newT = 0;
    if(fabs(dir.x()) > 1e-6){
        newT = (actual_hitpoint - r.getOrigin()).x() / r.getDirection().x();
    }
    else if (fabs(dir.y()) > 1e-6){
        newT = (actual_hitpoint - r.getOrigin()).y() / r.getDirection().y();
    }
    else {
        newT = (actual_hitpoint - r.getOrigin()).z() / r.getDirection().z();
    }

    if(newT < h.getT()){
        // 计算实际的 normal 
        Vec3f normal = new_h.getNormal();
        Vec4f normal_4(normal,0.0f);
        Matrix inv_m_trans = inv_m;
        inv_m_trans.Set(3, 0, 0);
        inv_m_trans.Set(3, 1, 0);
        inv_m_trans.Set(3, 2, 0); // 去除平移

        inv_m_trans.Transpose();

        inv_m_trans.Transform(normal_4);
        normal_4.Normalize();
        Vec3f transformed_normal(normal_4.x(),normal_4.y(),normal_4.z());

        h.set(newT, new_h.getMaterial(), transformed_normal, r);
    }

    return true; 
}

void Transform::paint(){
    glPushMatrix();
    GLfloat *glMatrix = this->m.glGet();
    glMultMatrixf(glMatrix);

    o->paint();

    glPopMatrix();
    delete[] glMatrix;
}