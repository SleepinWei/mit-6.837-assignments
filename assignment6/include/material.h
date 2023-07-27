#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "vectors.h"
// include glCanvas.h to access the preprocessor variable SPECULAR_FIX
#include "glCanvas.h"
#include "matrix.h"

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

class Material
{

public:
  // CONSTRUCTORS & DESTRUCTOR
  Material(const Vec3f &d_color) { diffuseColor = d_color; }
  virtual ~Material() {}

  // ACCESSORS
  virtual Vec3f getDiffuseColor(Vec3f p = {0,0,0}) const { return diffuseColor; }
  virtual Vec3f Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight,
                      const Vec3f &lightColor) const = 0;
  virtual void glSetMaterial(void) const = 0;

  virtual float getExponent() { return 0.0f; }

  virtual Vec3f getSpecularColor() { return {}; }

  virtual float getIOR() { return 0.0f; };

  virtual Vec3f getReflectiveColor() { return {}; }

  virtual Vec3f getTransparentColor() { return {}; }

  virtual void setDiffuseColor(Vec3f color) { diffuseColor = color; }

protected:
  // REPRESENTATION
  Vec3f diffuseColor;
};

class PhongMaterial : public Material
{
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
                const Vec3f &reflectiveColor,
                const Vec3f &transparentColor,
                float indexOfRefraction);

  virtual ~PhongMaterial();

  virtual Vec3f Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight,
                      const Vec3f &lightColor) const override;
  virtual void glSetMaterial(void) const;

  Vec3f getSpecularColor() const
  {
    return specular;
  }

  float getExponent() { return exponent; }

  float getIOR() { return ior; };

  Vec3f getReflectiveColor() { return reflectiveColor; }

  Vec3f getTransparentColor() { return transparentColor; }
};

// ====================================================================
// ====================================================================

class Checkerboard : public Material
{
public:
  Checkerboard(Matrix *trans, Material *mat1, Material *mat2) : trans(trans), mat1(mat1), mat2(mat2), Material({}){

                                                                                                      };
  ~Checkerboard(){};
  virtual void glSetMaterial(void) const;
  virtual Vec3f Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight,
                      const Vec3f &lightColor) const override;
  Material *getMaterialByPosition(Vec3f p)const{
      trans->Transform(p);
          int x = p.x(), y = p.y(), z = p.z();
      Material *mat = mat1;
      if ((x + y + z) % 2 == 1)
      {
        mat = mat2;
      }
      return mat; 
  }
  virtual Vec3f getDiffuseColor(Vec3f p = {0,0,0}) const override {
      auto mat = getMaterialByPosition(p);
      return mat->getDiffuseColor();
  }

public:
  Material *mat1;
  Material *mat2;
  Matrix *trans;
};

class Noise : public Material
{
public:
  Noise(Matrix *m, Material *mat1, Material *mat2, int octaves) : m(m), mat1(mat1), mat2(mat2), octaves(octaves), Material({})
  {
  }
  ~Noise(){};

  virtual void glSetMaterial(void) const { mat1->glSetMaterial(); }

  virtual Vec3f Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight,
                      const Vec3f &lightColor) const override;
  virtual Vec3f getDiffuseColor(Vec3f p = {0, 0, 0}) const override;

public:
  Matrix *m;
  Material *mat1;
  Material *mat2;
  int octaves;
};

class Marble: public Material
{
public:
  Marble(Matrix *m, Material *mat1, Material *mat2, int octaves,float frequency, float amplitude) : m(m), mat1(mat1), mat2(mat2), 
    octaves(octaves),frequency(frequency),amplitude(amplitude), Material({})
  {

  }
  ~Marble(){};

  virtual void glSetMaterial(void) const { mat1->glSetMaterial(); }

  virtual Vec3f Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight,
                      const Vec3f &lightColor) const override;
  virtual Vec3f getDiffuseColor(Vec3f p = {0, 0, 0}) const override;
public:
  Matrix *m;
  Material *mat1;
  Material *mat2;
  int octaves;
  float frequency;
  float amplitude;
};

class Wood: public Material
{
public:
  Wood(Matrix *m, Material *mat1, Material *mat2, int octaves,float frequency, float amplitude) : m(m), mat1(mat1), mat2(mat2), 
    octaves(octaves),frequency(frequency),amplitude(amplitude), Material({})
  {

  }
  ~Wood(){};

  virtual void glSetMaterial(void) const { mat1->glSetMaterial(); }

  virtual Vec3f Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight,
                      const Vec3f &lightColor) const override;
  virtual Vec3f getDiffuseColor(Vec3f p = {0, 0, 0}) const override;
public:
  Matrix *m;
  Material *mat1;
  Material *mat2;
  int octaves;
  float frequency;
  float amplitude;
};


#endif
