#include"raytracer.h"
#include"hit.h"
#include"raytracing_stats.h"
#include"matrix.h"
#include"ray.h"
#include"scene_parser.h"
#include"group.h"
#include"camera.h"
#include"light.h"
#include"Transform.h"
#include"material.h"
#include"rayTree.h"
#include"grid.h"
#include"object3d.h"

bool partial_ge(const Vec3f& v1, const Vec3f& v2){
    if(v1.x() >= v2.x() + 1e-6 || v1.y() >= v2.y() + 1e-6 || v1.z() >= v2.z() +  1e-6){
        return true; 
    }
    return false; 
}

bool transmittedDirection(const Vec3f &normal, const Vec3f &incoming, 
                            float index_i, float index_t, Vec3f &transmitted){

    // Vec3f N = normal;
    // float eta_r = index_i / index_t;
    // Vec3f I = (-1) * incoming;
    // I.Normalize();
    // float temp = 1 - eta_r * eta_r * (1 - N.Dot3(I) * N.Dot3(I));
    // if (temp < 0) {     /* internal reflection */
    //     transmitted = incoming - 2 * incoming.Dot3(N) * N;
    //     transmitted.Normalize();
    //     return true;
    // }
    // transmitted = (eta_r * N.Dot3(I) - sqrt(temp)) * N - eta_r * I;
    // transmitted.Normalize();
    // return false;
    auto revI = incoming;
    revI.Negate();
    float index_r = index_i / index_t;
    float nDotI = normal.Dot3(revI);
    float inSqrt = 1 - index_r * index_r * (1 - nDotI * nDotI);
    if (inSqrt < 0) return false;
    float coN = index_r * nDotI - sqrt(inSqrt), coI = -index_r;
    transmitted = coN * normal + coI * revI;
    transmitted.Normalize();
    return true;
}

Vec3f mirrorDirection(const Vec3f &normal, const Vec3f &incoming){
    float VdotN = incoming.Dot3(normal);
    Vec3f reflect = incoming - 2 * VdotN * normal;
    reflect.Normalize();
    return reflect;
}

extern bool enable_grid;
extern int nx, ny, nz; 

RayTracer::RayTracer(SceneParser *s, int max_bounces, float cutoff_weight, bool shadows){
    this->parser = s;
    this->max_bounces = max_bounces;
    this->cutoff_weight = cutoff_weight;
    this->shadows = shadows;

    auto group =(Group*) s->getGroup();
    if(!group->getBoundingBox())
        group->generateBoundingBox();

    // if(enable_grid){
        grid = new Grid(group->getBoundingBox(),nx,ny,nz);
        Matrix identity;
        identity.SetToIdentity();
        group->insertIntoGrid(grid,&identity);

        int sum_max = 0;
        for (int i = 0; i < grid->nx;i++){
            for (int j = 0; j < grid->ny;j++){
                for (int k = 0; k < grid->nz;k++){
                    sum_max = std::max(sum_max, (int)grid->arr[i][j][k].size());
                }
            }
        }
        grid->scale = sum_max - 1;
        if(grid->scale == 0){
            grid->scale = 1; 
        }
    // }
}

RayTracer::~RayTracer(){
    if(grid){
        delete grid; 
    }
}

extern bool shade_back;
extern bool shadows;
extern bool visualize_grid;
extern bool enable_grid; 

bool RayTracer::RayCast(Ray& ray, Hit& hit, float tmin,Group* group,bool shadowRay)const {
    if(!enable_grid)
        return group->intersect(ray,hit,tmin);
    else if(enable_grid && !visualize_grid || enable_grid && visualize_grid && shadowRay){
        return RayCastFast(ray, hit, tmin, group);
    }
    else if (visualize_grid){
        return grid->intersect(ray, hit, tmin);
    }
    else {
        cout << "wrong raycast choice\n";
    }
}

bool RayTracer::RayCastFast(Ray &ray, Hit &hit, float tmin,Group* group) const
{
    bool actual_intersect = false; 
    for(auto object : grid->other_objects){
        actual_intersect |= object->intersect(ray, hit, tmin);
    }

    Hit grid_h(INFINITY,nullptr,{}); 
    bool isIntersect = grid->intersect(ray, grid_h, tmin);
    auto info = grid->info;

    bool grid_intersect = false; 
    while (info.i >= 0 && info.i < nx && info.j >= 0 && info.j < ny && info.k >= 0 && info.k < nz)
    {
        for(auto object: grid->arr[info.i][info.j][info.k]){
            grid_intersect |= object->intersect(ray, hit, tmin);
        }

        if(grid_intersect){ // 找到了 grid 里相交的物体，并和 不在 grid 里的物体求较小值
            return true;
        }

        info.nextCell(); // 继续走下一个 grid
    }

    return actual_intersect;
}

Vec3f RayTracer::traceRay(Ray &ray, float tmin, int bounces, float weight, 
                 float indexOfRefraction, Hit &hit) const
{
    RayTracingStats::IncrementNumNonShadowRays();

    if(bounces > max_bounces || weight < cutoff_weight){
        return {0.0f,0.0f,0.0f}; 
    }

    auto group = parser->getGroup();
    auto camera = parser->getCamera();
    bool group_intersect = RayCast(ray,hit,tmin,group,false);
    // if(visualize_grid){
        // group_intersect = grid->intersect(ray, hit, tmin);
    // }
    // else {
        // group_intersect = group->intersect(ray, hit, tmin);
    // }
    if(!group_intersect){
        return parser->getBackgroundColor();
    }

    // handling back-facing
    bool isInside = hit.getNormal().Dot3(ray.getDirection()) > 0.0f ;
    // handling back-facing
    if(shade_back && isInside){
        auto n = hit.getNormal();
        hit.setNormal({-n.x(), -n.y(), -n.z()});
    }

    auto mat =(PhongMaterial*)hit.getMaterial();

    // shading 
    Vec3f Color = parser->getAmbientLight() * hit.getMaterial()->getDiffuseColor();
    for (int i = 0; i < parser->getNumLights();i++){
        auto light = parser->getLight(i);
        Vec3f dirToLight;
        Vec3f lightColor;
        Vec3f intersection = hit.getIntersectionPoint();
        float distanceToLight; 

        light->getIllumination(intersection, dirToLight, lightColor,distanceToLight);

        if(shadows){
            // 渲染阴影
            Hit H_light(INFINITY,nullptr,{});

            float epsilon = 1e-2;

            RayTracingStats::IncrementNumShadowRays();
            Ray toLight(hit.getIntersectionPoint() + epsilon * dirToLight, dirToLight);
            bool IsIntersect = RayCast(toLight,H_light,0.0f,group,true);
            // group->intersect(toLight, H_light, 0.0f);
            // if(distanceToLight==INFINITY && !intersect || distanceToLight < INFINITY && fabs(H_light.getT()-distanceToLight) <= 1e-2){
            
            if(!IsIntersect || IsIntersect && H_light.getT() >= distanceToLight){
                Color += mat->Shade(ray, hit, dirToLight, lightColor);
            }
        }
        else {
            // 不渲染阴影
            Color += mat->Shade(ray, hit, dirToLight, lightColor);
        }
    }

    //reflectance
    if(partial_ge(mat->getReflectiveColor(),Vec3f{0.0f,0.0f,0.0f})){
        Vec3f reflect_dir = mirrorDirection(hit.getNormal(), ray.getDirection());
        Ray reflect_ray(hit.getIntersectionPoint() + reflect_dir * 1e-4, reflect_dir);

        Hit newHit(INFINITY,nullptr,{}); 
        Vec3f reflect_color = traceRay(reflect_ray, tmin, bounces + 1, weight * hit.getMaterial()->getReflectiveColor().Length(), 
            indexOfRefraction, newHit);

        Color += reflect_color * mat->getReflectiveColor();
    }

    //refraction 
    if(partial_ge(mat->getTransparentColor(),{0.0f,0.0f,0.0f})){
        Vec3f refracted_dir;
        float ior_i = 1.0f;
        float ior_r = mat->getIOR(); 
        if(isInside){
            std::swap(ior_i, ior_r);
        }
        bool hasRefraction = transmittedDirection(hit.getNormal(), ray.getDirection(), ior_i, ior_r, refracted_dir);
        if(hasRefraction){
            Ray refract_ray(hit.getIntersectionPoint() + refracted_dir * 1e-4, refracted_dir);

            Hit newHit(INFINITY,nullptr,{});
            Vec3f refract_color = traceRay(refract_ray, tmin, bounces + 1, weight * mat->getTransparentColor().Length(), 
                mat->getIOR(), newHit);
            Color += refract_color * hit.getMaterial()->getTransparentColor();
        }
    }

    return Color; 
}

void RayTracer::traceSingleRay(Ray &ray, float tmin, int bounces, float weight, 
                 float indexOfRefraction, Hit &hit) const
{
    if(bounces > max_bounces || weight < cutoff_weight){
        return; 
    }

    auto group = parser->getGroup();
    auto camera = parser->getCamera();
    bool intersect = group->intersect(ray, hit, camera->getTMin());
    if(!intersect){
        return; 
    }

    bool isInside = hit.getNormal().Dot3(ray.getDirection()) > 0.0f ;
    // handling back-facing
    if(shade_back && isInside){
        auto n = hit.getNormal();
        hit.setNormal({-n.x(), -n.y(), -n.z()});
    }

    auto mat =(PhongMaterial*)hit.getMaterial();

    // shading 
    for (int i = 0; i < parser->getNumLights();i++){
        auto light = parser->getLight(i);
        Vec3f dirToLight;
        Vec3f lightColor;
        Vec3f intersection = hit.getIntersectionPoint();
        float distanceToLight; 

        light->getIllumination(intersection, dirToLight, lightColor,distanceToLight);

        if(shadows){
            // 渲染阴影
            Hit H_light(INFINITY,nullptr,{});

            float epsilon = 1e-2;
            Ray toLight(hit.getIntersectionPoint() + epsilon * dirToLight, dirToLight);
            
            bool intersect = group->intersect(toLight, H_light, tmin);
            RayTree::AddShadowSegment(toLight, 0, H_light.getT());
        }
    }

    //reflectance
    if(partial_ge(mat->getReflectiveColor(),Vec3f{0.0f,0.0f,0.0f})){
        Vec3f reflect_dir = mirrorDirection(hit.getNormal(), ray.getDirection());
        Ray reflect_ray(hit.getIntersectionPoint() + reflect_dir * 1e-4, reflect_dir);

        Hit newHit(INFINITY,nullptr,{}); 
        traceSingleRay(reflect_ray, tmin, bounces + 1, weight * hit.getMaterial()->getReflectiveColor().Length(), 
            indexOfRefraction, newHit);
        RayTree::AddReflectedSegment(reflect_ray, 0, newHit.getT());
    }

    //refraction 
    if(partial_ge(mat->getTransparentColor(),{0.0f,0.0f,0.0f})){
        Vec3f refracted_dir;
        float ior_i = 1.0f;
        float ior_r = mat->getIOR(); 
        if(isInside){
            std::swap(ior_i, ior_r);
        }
        transmittedDirection(hit.getNormal(), ray.getDirection(), ior_i, ior_r, refracted_dir);

        Hit newHit(INFINITY,nullptr,{});
        Ray refract_ray(hit.getIntersectionPoint() + refracted_dir * 1e-4, refracted_dir);
        traceSingleRay(refract_ray, tmin, bounces + 1, weight * mat->getTransparentColor().Length(), mat->getIOR(), newHit);
        RayTree::AddTransmittedSegment(refract_ray, 0, newHit.getT());
    }

    return;
}