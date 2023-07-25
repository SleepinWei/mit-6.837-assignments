#include"raytracer.h"
#include"hit.h"
#include"ray.h"
#include"scene_parser.h"
#include"group.h"
#include"camera.h"
#include"light.h"
#include"material.h"
#include"rayTree.h"

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

RayTracer::RayTracer(SceneParser *s, int max_bounces, float cutoff_weight, bool shadows){
    this->parser = s;
    this->max_bounces = max_bounces;
    this->cutoff_weight = cutoff_weight;
    this->shadows = shadows;
}

extern bool shade_back;
extern bool shadows; 

Vec3f RayTracer::traceRay(Ray &ray, float tmin, int bounces, float weight, 
                 float indexOfRefraction, Hit &hit) const
{
    if(bounces > max_bounces || weight < cutoff_weight){
        return {0.0f,0.0f,0.0f}; 
    }

    auto group = parser->getGroup();
    auto camera = parser->getCamera();
    bool group_intersect = group->intersect(ray, hit, tmin);
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
            Ray toLight(hit.getIntersectionPoint() + epsilon * dirToLight, dirToLight);
            bool IsIntersect = group->intersect(toLight, H_light, 0.0f);
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