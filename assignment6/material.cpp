#include "material.h"
#include "hit.h"
#include "ray.h"
#include"perlin_noise.h"
#include <GL/freeglut.h>

PhongMaterial::PhongMaterial(
    const Vec3f &diffuseColor,
    const Vec3f &specularColor,
    float exponent,
    const Vec3f &reflectiveColor,
    const Vec3f &transparentColor,
    float indexOfRefraction) : specular(specularColor), exponent(exponent), Material(diffuseColor),
                               reflectiveColor(reflectiveColor), transparentColor(transparentColor),
                               ior(indexOfRefraction)
{
}

PhongMaterial::~PhongMaterial()
{
}

void PhongMaterial::glSetMaterial(void) const
{

  GLfloat one[4] = {1.0, 1.0, 1.0, 1.0};
  GLfloat zero[4] = {0.0, 0.0, 0.0, 0.0};
  GLfloat specular[4] = {
      getSpecularColor().r(),
      getSpecularColor().g(),
      getSpecularColor().b(),
      1.0};
  GLfloat diffuse[4] = {
      getDiffuseColor().r(),
      getDiffuseColor().g(),
      getDiffuseColor().b(),
      1.0};

  // NOTE: GL uses the Blinn Torrance version of Phong...
  float glexponent = exponent;
  if (glexponent < 0)
    glexponent = 0;
  if (glexponent > 128)
    glexponent = 128;

#if !SPECULAR_FIX

  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, diffuse);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &glexponent);

#else

  // OPTIONAL: 3 pass rendering to fix the specular highlight
  // artifact for small specular exponents (wide specular lobe)

  if (SPECULAR_FIX_WHICH_PASS == 0)
  {
    // First pass, draw only the specular highlights
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, zero);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, zero);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &glexponent);
  }
  else if (SPECULAR_FIX_WHICH_PASS == 1)
  {
    // Second pass, compute normal dot light
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, one);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, zero);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, zero);
  }
  else
  {
    // Third pass, add ambient & diffuse terms
    assert(SPECULAR_FIX_WHICH_PASS == 2);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, zero);
  }

#endif
}

Vec3f PhongMaterial::Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight,
                           const Vec3f &lightColor) const
{
  Vec3f result(0.0f, 0.0f, 0.0f);

  Vec3f v;
  v -= ray.getDirection(); // -dir
  Vec3f normal = hit.getNormal();

  Vec3f h = 0.5f * (dirToLight + v);
  h.Normalize();

  float NdotH = max(h.Dot3(normal), 0.0f);

  // specular
  result += lightColor * powf(NdotH, exponent) * specular;

  // diffuse
  float NdotL = max(normal.Dot3(dirToLight), 0.0f);
  result += getDiffuseColor() * NdotL * lightColor;

  return result;
}

void Checkerboard::glSetMaterial() const
{
  mat1->glSetMaterial();
}

Vec3f Checkerboard::Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight,
                          const Vec3f &lightColor) const
{
  Vec3f p = hit.getIntersectionPoint();
  this->trans->Transform(p);
  int x = p.x(), y = p.y(), z = p.z();
  Material *mat = mat1;
  if ((x + y + z) % 2 == 1)
  {
    mat = mat2;
  }

  return mat->Shade(ray, hit, dirToLight, lightColor);
}

Vec3f Noise::Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight,
                   const Vec3f &lightColor) const
{
  Vec3f p = hit.getIntersectionPoint();
  this->m->Transform(p);

  float N = 0;
  for (int i = 0; i < octaves;i++){
    float coeff = 2 << i;
    N += PerlinNoise::noise(coeff * p.x(), coeff * p.y(), coeff * p.z()) / coeff; 
  }
  N = std::min(std::max(N, 0.0f), 1.0f); // clamp to 0~1
  Vec3f color1 = mat1->Shade(ray, hit, dirToLight, lightColor);
  Vec3f color2 = mat2->Shade(ray, hit, dirToLight, lightColor);
  Vec3f Color = color1 * N + color2 * (1 - N);
  return Color;
}

Vec3f Noise::getDiffuseColor(Vec3f p) const{
  this->m->Transform(p);

  float N = 0;
  for (int i = 0; i < octaves;i++){
    float coeff = 2 << i;
    N += PerlinNoise::noise(coeff * p.x(), coeff * p.y(), coeff * p.z()) / coeff; 
  }
  N = std::min(std::max(N, 0.0f), 1.0f); // clamp to 0~1
  return mat1->getDiffuseColor() * N + mat2->getDiffuseColor() * (1 - N);
}

Vec3f Marble::Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight,
                   const Vec3f &lightColor) const
{
  Vec3f p = hit.getIntersectionPoint();
  this->m->Transform(p);

  float N = 0;
  for (int i = 0; i < octaves;i++){
    float coeff = 2 << i;
    N += PerlinNoise::noise(coeff * p.x(), coeff * p.y(), coeff * p.z()) / coeff; 
  }
  float M = sin(frequency * p.x() + amplitude * N);
  Vec3f color1 = mat1->Shade(ray, hit, dirToLight, lightColor);
  Vec3f color2 = mat2->Shade(ray, hit, dirToLight, lightColor);
  Vec3f Color = color1 * M + color2 * (1 - M);
  return Color;
}

Vec3f Marble::getDiffuseColor(Vec3f p) const{
  this->m->Transform(p);

  float N = 0;
  for (int i = 0; i < octaves;i++){
    float coeff = 2 << i;
    N += PerlinNoise::noise(coeff * p.x(), coeff * p.y(), coeff * p.z()) / coeff; 
  }
  float M = sin(frequency * p.x() + amplitude * N);
  return mat1->getDiffuseColor() * M + mat2->getDiffuseColor() * (1 - M);
}

Vec3f Wood::Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight,
                   const Vec3f &lightColor) const
{
  Vec3f p = hit.getIntersectionPoint();
  this->m->Transform(p);

  float N = 0;
  for (int i = 0; i < octaves;i++){
    float coeff = 2 << i;
    N += PerlinNoise::noise(coeff * p.x(), coeff * p.y(), coeff * p.z()) / coeff; 
  }
  float M = sin(frequency * p.x() + amplitude * N);
  Vec3f color1 = mat1->Shade(ray, hit, dirToLight, lightColor);
  Vec3f color2 = mat2->Shade(ray, hit, dirToLight, lightColor);
  Vec3f Color = color1 * M + color2 * (1 - M);
  return Color;
}

Vec3f Wood::getDiffuseColor(Vec3f p) const{
  this->m->Transform(p);

  float N = 0;
  for (int i = 0; i < octaves;i++){
    float coeff = 2 << i;
    N += PerlinNoise::noise(coeff * p.x(), coeff * p.y(), coeff * p.z()) / coeff; 
  }
  float M = sin(frequency * p.x() + amplitude * N);
  return mat1->getDiffuseColor() * M + mat2->getDiffuseColor() * (1 - M);
}
