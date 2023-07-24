#include"vectors.h"
class SceneParser; 
class Material;
class Hit; 

class Shading{
public:
    virtual Vec3f exec(Material* m,Hit& h,SceneParser& parser) = 0;
};

class NormalShading: public Shading{
public:
    virtual Vec3f exec(Material *m, Hit &h, SceneParser& parser) override;
};

class DiffuseShading: public Shading{
    virtual Vec3f exec(Material *m, Hit &h,SceneParser& parser) override;
};