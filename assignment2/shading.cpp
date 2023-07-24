#include"shading.h"
#include"material.h"
#include"hit.h"
#include"light.h"
#include"scene_parser.h"

Vec3f NormalShading::exec(Material* m,Hit& h,SceneParser& parser){
    Vec3f normal = h.getNormal();
    normal.Set(fabs(normal.x()), fabs(normal.y()), fabs(normal.z()));

    return normal;
}

Vec3f DiffuseShading::exec(Material* m,Hit& h,SceneParser& parser){
    Vec3f normal = h.getNormal();
    Vec3f result(0.0f,0.0f,0.0f);

    Vec3f ambient = parser.getAmbientLight();

    result += ambient * m->getDiffuseColor(); 

    int light_num = parser.getNumLights();

    for (int i = 0; i < light_num;i++){
        Light *light = parser.getLight(i);
        // directional
        Vec3f L;
        Vec3f light_color;
        light->getIllumination(h.getIntersectionPoint(), L, light_color);
        result += std::max(0.0f, normal.Dot3(L)) * light_color * m->getDiffuseColor();
    }

    return result; 
}