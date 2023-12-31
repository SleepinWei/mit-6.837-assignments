#include<GL/freeglut.h>
#include"material.h"
#include"sphere.h"
#include"ray.h"
#include"boundingbox.h"
#include"grid.h"
#include"hit.h"

Sphere::Sphere(Vec3f center, float radius, Material *mat){
    this->center = center;
    this->radius = radius;
    this->mat = mat;
    bb = new BoundingBox(center - Vec3f(radius, radius, radius), center + Vec3f(radius, radius, radius));
}

Sphere::~Sphere(){
    delete bb; 
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
    Vec3f pos(sin(theta_y) * sin(theta_x),cos(theta_y),sin(theta_y) * cos(theta_x));
    return pos;
}

extern int tessx, tessy;
extern bool enable_gouraud;

void Sphere::paint(){
    const float pi = 3.14f;

    Vec3f color = this->mat->getDiffuseColor();
    Vec3f specular = this->mat->getSpecularColor();
    // glColor3f(color.x(), color.y(), color.z());
    // geometry
    mat->glSetMaterial();

    glBegin(GL_QUADS);
    for (int i = 0; i < tessy; ++i){
        for (int j = 0; j < tessx;++j){
            float theta_y = pi * i / tessy; 
            float theta_x = 2 * pi / tessx * j;

            Vec3f pos1 = getPositionOnSphere(theta_x, theta_y);
            Vec3f pos2 = getPositionOnSphere(theta_x, theta_y + pi / tessy);
            Vec3f pos3 = getPositionOnSphere(theta_x + 2 * pi / tessx, theta_y + pi / tessy);
            Vec3f pos4 = getPositionOnSphere(theta_x + 2 * pi / tessx, theta_y);

            Vec3f normal;
            Vec3f::Cross3(normal, pos2 - pos1, pos3 - pos1);
            normal.Normalize();

            glNormal3f(normal.x(),normal.y(),normal.z());
            if(enable_gouraud){
                glNormal3f(pos1.x(),pos1.y(),pos1.z());
            }
            pos1 *= radius; 
            pos1 += center; 
            glVertex3f(pos1.x(),pos1.y(),pos1.z());

            if(enable_gouraud){
                glNormal3f(pos2.x(),pos2.y(),pos2.z());
            }
            pos2 *= radius;
            pos2 += center; 
            glVertex3f(pos2.x(),pos2.y(),pos2.z());

            if(enable_gouraud){
                glNormal3f(pos3.x(),pos3.y(),pos3.z());
            }
            pos3 *= radius;
            pos3 += center;
            glVertex3f(pos3.x(),pos3.y(),pos3.z());

            if(enable_gouraud){
                glNormal3f(pos4.x(),pos4.y(),pos4.z());
            }
            pos4 *= radius;
            pos4 += center;
            glVertex3f(pos4.x(),pos4.y(),pos4.z());
        }
    }
    glEnd();
}

void Sphere::insertIntoGrid(Grid *g, Matrix *m){
    if(m){
        Vec3f newCenter = center;
        m->Transform(newCenter);

        Vec4f rad_vec(radius, 0.0f, 0.0f,0.0f);
        Matrix newM = *m;
        newM.Set(3, 0, 0);
        newM.Set(3, 1, 0);
        newM.Set(3, 2, 0);

        m->Transform(rad_vec);
        float newRadius = rad_vec.Length();

        delete bb;
        bb = new BoundingBox(newCenter - Vec3f{newRadius,newRadius,newRadius},
            newCenter + Vec3f{newRadius,newRadius,newRadius});
    }

    auto bounding_box = g->getBoundingBox();
    Vec3f max_pos = bounding_box->getMax();
    Vec3f min_pos = bounding_box->getMin();
    Vec3f size = max_pos - min_pos;
    size.Divide(g->nx, g->ny, g->nz);

    auto bb_min = bb->getMin();
    auto bb_max = bb->getMax();

    Vec3f min_grid = bb_min - min_pos, max_grid = bb_max - min_pos;
    min_grid.Divide(size.x(), size.y(), size.z());
    max_grid.Divide(size.x(), size.y(), size.z());
    for (int i = std::max(0.0f, floor(min_grid.x())); i <= std::min(g->nx * 1.0f - 1, (max_grid.x())); i++)
    {
        for (int j = std::max(0.0f, floor(min_grid.y())); j <= std::min(g->ny * 1.0f -1, (max_grid.y())); j++)
        {
            for (int k = std::max(0.0f, floor(min_grid.z())); k <= std::min(g->nz * 1.0f -1, (max_grid.z())); k++)
            {
                g->arr[i][j][k].push_back(this);
            }
        }
    }
    // for (int i = 0; i < g->nx;i++){
        // for (int j = 0; j < g->ny;j++){
            // for (int k = 0; k < g->nz;k++){
                // Vec3f pos;
                // Vec3f::Mult(pos, size, {i, j, k});
                // pos += min_pos;
                // Vec3f pos_center = 0.5f * size + pos; // 中心
                // float half_len = 0.5f * sqrtf(size.Dot3(size));

                // if((pos_center - center).Length() - half_len <= radius){
                //     g->arr[i][j][k].push_back(this); 
                // }
            // }
        // }
    // }
}