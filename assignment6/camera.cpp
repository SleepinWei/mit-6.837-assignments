#include"camera.h"
#include"ray.h"
#include"matrix.h"

void OrthographicCamera::glInit(int w, int h)
{
    float size = this->image_size;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (w > h)
        glOrtho(-size / 2.0, size / 2.0, -size * (float)h / (float)w / 2.0, size * (float)h / (float)w / 2.0, 0.5, 40.0);
    else
        glOrtho(-size * (float)w / (float)h / 2.0, size * (float)w / (float)h / 2.0, -size / 2.0, size / 2.0, 0.5, 40.0);
}

void OrthographicCamera::glPlaceCamera(void)
{
  gluLookAt(center.x(), center.y(), center.z(),
            center.x()+direction.x(), center.y()+direction.y(), center.z()+direction.z(),
            up.x(), up.y(), up.z());
}

// ====================================================================
// dollyCamera, truckCamera, and RotateCamera
//
// Asumptions:
//  - up is really up (i.e., it hasn't been changed
//    to point to "screen up")
//  - up and direction are normalized
// Special considerations:
//  - If your constructor precomputes any vectors for
//    use in 'generateRay', you will likely to recompute those
//    values at athe end of the these three routines
// ====================================================================

// ====================================================================
// dollyCamera: Move camera along the direction vector
// ====================================================================

void OrthographicCamera::dollyCamera(float dist)
{
  center += direction*dist;

  // ===========================================
  // ASSIGNMENT 3: Fix any other affected values
  // ===========================================


}

// ====================================================================
// truckCamera: Translate camera perpendicular to the direction vector
// ====================================================================

void OrthographicCamera::truckCamera(float dx, float dy)
{
  Vec3f horizontal;
  Vec3f::Cross3(horizontal, direction, up);
  horizontal.Normalize();

  Vec3f screenUp;
  Vec3f::Cross3(screenUp, horizontal, direction);

  center += horizontal*dx + screenUp*dy;

  // ===========================================
  // ASSIGNMENT 3: Fix any other affected values
  // ===========================================


}


// ====================================================================
// rotateCamera: Rotate around the up and horizontal vectors
// ====================================================================

void OrthographicCamera::rotateCamera(float rx, float ry)
{
  Vec3f horizontal;
  Vec3f::Cross3(horizontal, direction, up);
  horizontal.Normalize();

  // Don't let the model flip upside-down (There is a singularity
  // at the poles when 'up' and 'direction' are aligned)
  float tiltAngle = acos(up.Dot3(direction));
  if (tiltAngle-ry > 3.13)
    ry = tiltAngle - 3.13;
  else if (tiltAngle-ry < 0.01)
    ry = tiltAngle - 0.01;

  Matrix rotMat = Matrix::MakeAxisRotation(up, rx);
  rotMat *= Matrix::MakeAxisRotation(horizontal, ry);

  rotMat.Transform(center);
  rotMat.TransformDirection(direction);
  
  // ===========================================
  // ASSIGNMENT 3: Fix any other affected values
  // ===========================================
//   rotMat.TransformDirection(up);
//   rotMat.TransformDirection(horizontal);
//   up.Normalize();
//   horizontal.Normalize();
}

OrthographicCamera::OrthographicCamera(Vec3f center, Vec3f direction, Vec3f up, float image_size){
    this->center = center;

    this->direction = direction;
    this->direction.Normalize();

    this->up = up;
    this->up.Normalize();

    this->image_size = image_size;
}

float OrthographicCamera::getTMin() const{
    return -1e6; 
}

Ray OrthographicCamera::generateRay(Vec2f point){
    Vec3f horizontal;
    Vec3f::Cross3(horizontal, direction, up);
    horizontal.Normalize();

    Vec3f _up;
    Vec3f::Cross3(_up, horizontal, direction);
    _up.Normalize();

    Vec3f origin = center;
    point *= 2; 
    Vec2f::Sub(point, point, {1.0f, 1.0f}); // -1 ~ 1

    origin += 0.5 * point.x() * horizontal * image_size + point.y() * _up * 0.5 * image_size;

    return Ray(origin, this->direction);
}

void PerspectiveCamera::glInit(int w, int h)
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(angle*180.0/3.14159, (float)w/float(h), 0.5, 40.0);
}

// ====================================================================
// Place a perspective camera within an OpenGL scene
// ====================================================================

void PerspectiveCamera::glPlaceCamera(void)
{
  gluLookAt(center.x(), center.y(), center.z(),
            center.x()+direction.x(), center.y()+direction.y(), center.z()+direction.z(),
            up.x(), up.y(), up.z());
}

// ====================================================================
// dollyCamera, truckCamera, and RotateCamera
//
// Asumptions:
//  - up is really up (i.e., it hasn't been changed
//    to point to "screen up")
//  - up and direction are normalized
// Special considerations:
//  - If your constructor precomputes any vectors for
//    use in 'generateRay', you will likely to recompute those
//    values at athe end of the these three routines
// ====================================================================

// ====================================================================
// dollyCamera: Move camera along the direction vector
// ====================================================================

void PerspectiveCamera::dollyCamera(float dist)
{
  center += direction*dist;

  // ===========================================
  // ASSIGNMENT 3: Fix any other affected values
  // ===========================================


}

// ====================================================================
// truckCamera: Translate camera perpendicular to the direction vector
// ====================================================================

void PerspectiveCamera::truckCamera(float dx, float dy)
{
  Vec3f horizontal;
  Vec3f::Cross3(horizontal, direction, up);
  horizontal.Normalize();

  Vec3f screenUp;
  Vec3f::Cross3(screenUp, horizontal, direction);

  center += horizontal*dx + screenUp*dy;

  // ===========================================
  // ASSIGNMENT 3: Fix any other affected values
  // ===========================================


}

// ====================================================================
// rotateCamera: Rotate around the up and horizontal vectors
// ====================================================================

void PerspectiveCamera::rotateCamera(float rx, float ry)
{
//   Vec3f _up(0.0f, 1.0f, 0.0f);
  Vec3f horizontal;
  Vec3f::Cross3(horizontal, direction, up);
  horizontal.Normalize();

  // Don't let the model flip upside-down (There is a singularity
  // at the poles when 'up' and 'direction' are aligned)
  float tiltAngle = acos(up.Dot3(direction));
  if (tiltAngle-ry > 3.13)
    ry = tiltAngle - 3.13;
  else if (tiltAngle-ry < 0.01)
    ry = tiltAngle - 0.01;

  Matrix rotMat = Matrix::MakeAxisRotation(up, rx);
  rotMat *= Matrix::MakeAxisRotation(horizontal, ry);

  rotMat.Transform(center);
  rotMat.TransformDirection(direction);
  direction.Normalize();

  // ===========================================
  // ASSIGNMENT 3: Fix any other affected values
  // ===========================================

//   rotMat.TransformDirection(up);
//   rotMat.TransformDirection(horizontal);
//   up.Normalize();
//   Vec3f::Cross3(horizontal, direction, up);
//   horizontal.Normalize();
}

PerspectiveCamera::PerspectiveCamera(Vec3f& center, Vec3f& direction, Vec3f&up,float angle){
    this->center = center;

    direction.Normalize();
    this->direction = direction;

    up.Normalize();
    this->up = up; 

    // Vec3f::Cross3(this->horizontal, direction, up);
    // this->horizontal.Normalize();

    // Vec3f::Cross3(this->up, this->horizontal, direction);
    // this->up.Normalize();

    this->angle = angle;
}

float PerspectiveCamera::getTMin() const{
    return 0; 
}

Ray PerspectiveCamera::generateRay(Vec2f point){
    Vec3f horizontal;
    Vec3f::Cross3(horizontal, direction, up);
    horizontal.Normalize();

    Vec3f _up;
    Vec3f::Cross3(_up, horizontal, direction);
    _up.Normalize();

    point *= 2.0f;
    point -= {1.0f,1.0f}; // -1 ~ 1

    // float tan_half_angle = tanf(0.5f * this->angle);
    point *= 0.5 * this->angle;

    Vec3f ray_dir = this->direction;
    ray_dir += _up * tanf(point.y());
    ray_dir += horizontal * tanf(point.x());

    ray_dir.Normalize(); // normalization

    const float eps = 1e-4; 
    float x = ray_dir.x(), y = ray_dir.y(), z = ray_dir.z();
    if(x==0){
      x = eps; 
    }
    if(y == 0)
      y = eps;
    if(z==0)
      z = eps;
    ray_dir.Set(x, y, z);

    // cout << ray_dir << '\n';

    return Ray(this->center, ray_dir);
}