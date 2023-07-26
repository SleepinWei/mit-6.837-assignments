#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "vectors.h"
// include glCanvas.h to access the preprocessor variable SPECULAR_FIX
#include "glCanvas.h"  

#ifdef SPECULAR_FIX
// OPTIONAL:  global variable allows (hacky) communication 
// with glCanvas::display
extern int SPECULAR_FIX_WHICH_PASS;
#endif

// ====================================================================
// ====================================================================

// You will extend this class in later assignments

class Ray;
class Hit;

class Material {

public:

  // CONSTRUCTORS & DESTRUCTOR
  Material(const Vec3f &d_color) { diffuseColor = d_color; }
  virtual ~Material() {}

  // ACCESSORS
  virtual Vec3f getDiffuseColor() const { return diffuseColor; }
  virtual Vec3f Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight, 
     const Vec3f &lightColor) const = 0;
  virtual void glSetMaterial(void) const = 0;

  virtual float getExponent() { return 0.0f; }

  virtual Vec3f getSpecularColor() { return {}; }

  virtual float getIOR() { return 0.0f; };

  virtual Vec3f getReflectiveColor() { return {}; }

  virtual Vec3f getTransparentColor() { return {}; }

  virtual Vec3f setDiffuseColor(Vec3f color) { diffuseColor = color; }

protected:

  // REPRESENTATION
  Vec3f diffuseColor;
  
};

class PhongMaterial: public Material{
public:
  float exponent;
  Vec3f specular;
  Vec3f reflectiveColor;
  Vec3f transparentColor;
  float ior;

public:
  PhongMaterial(const Vec3f &diffuseColor, 
    const Vec3f &specularColor, 
    float exponent,
    const Vec3f& reflectiveColor,
    const Vec3f& transparentColor,
    float indexOfRefraction);

  virtual ~PhongMaterial();

  virtual Vec3f Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight,
                                const Vec3f &lightColor) const override;
  virtual void glSetMaterial(void) const;

  Vec3f getSpecularColor() const{
    return specular; 
  }

  float getExponent() { return exponent; }

  float getIOR() { return ior; };

  Vec3f getReflectiveColor() { return reflectiveColor; }

  Vec3f getTransparentColor() { return transparentColor; }
};

// ====================================================================
// ====================================================================

#endif
