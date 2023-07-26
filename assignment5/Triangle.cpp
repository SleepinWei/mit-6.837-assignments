#include "triangle.h"
#include "boundingbox.h"
#include "grid.h"
#include "hit.h"
#include "material.h"
#include "matrix.h"
#include "ray.h"
#include <GL/freeglut.h>

Triangle::Triangle(Vec3f &a, Vec3f &b, Vec3f &c, Material *m)
{
    this->a = a;
    this->b = b;
    this->c = c;
    this->mat = m;

    Vec3f::Cross3(normal, b - a, c - a);
    normal.Normalize();

    Vec3f pos_min;
    Vec3f pos_max;

    Vec3f::Min(pos_min, a, b);
    Vec3f::Min(pos_min, pos_min, c);

    Vec3f::Max(pos_max, a, b);
    Vec3f::Max(pos_max, pos_max, c);

    bb = new BoundingBox(pos_min, pos_max);
}

Triangle::~Triangle()
{
    delete bb;
}

bool Triangle::intersect(const Ray &r, Hit &h, float tmin)
{
    Vec3f origin = r.getOrigin();
    Vec3f d = r.getDirection();

    Vec3f E1 = b - a;
    Vec3f E2 = c - a;
    Vec3f T = origin - a;
    Vec3f P;
    Vec3f::Cross3(P, d, E2);
    Vec3f Q;
    Vec3f::Cross3(Q, T, E1);

    float denominator = P.Dot3(E1);

    float t = Q.Dot3(E2) / denominator;
    float u = P.Dot3(T) / denominator;
    float v = Q.Dot3(d) / denominator;

    // decide inside or not
    if (t < tmin || u < 0 || u > 1 || v < 0 || v > 1 || u + v > 1)
    {
        return false;
    }

    if (t < h.getT())
    {
        Vec3f normal;
        Vec3f::Cross3(normal, E1, E2);
        normal.Normalize();
        h.set(t, this->mat, normal, r);
    }

    return true;
}

void Triangle::paint()
{
    Vec3f color = this->mat->getDiffuseColor();
    Vec3f specular = this->mat->getSpecularColor();
    // glColor3f(color.x(), color.y(), color.z());
    // geometry
    mat->glSetMaterial();

    glBegin(GL_TRIANGLES);
    glNormal3f(normal.x(), normal.y(), normal.z());
    glColor3f(1.0f, 0.0f, 0.0f);

    glVertex3f(a.x(), a.y(), a.z());
    glVertex3f(b.x(), b.y(), b.z());
    glVertex3f(c.x(), c.y(), c.z());
    glEnd();
}

bool IsInsideTriangle(Vec3f p, Vec3f a, Vec3f b, Vec3f c)
{
    Vec3f pa = a - p;
    Vec3f pb = b - p;
    Vec3f pc = c - p;
    Vec3f ba = b - a;
    Vec3f cb = c - b;
    Vec3f ac = a - c;

    Vec3f cross_a, cross_b, cross_c;
    Vec3f cross_ab;
    Vec3f tri_normal;
    Vec3f::Cross3(cross_a, pa, ba);
    Vec3f::Cross3(cross_b, pb, cb);
    Vec3f::Cross3(cross_c, pc, ac);
    Vec3f::Cross3(cross_ab, pa, pb);
    Vec3f::Cross3(tri_normal, ba, cb);
    Vec3f::Cross3(tri_normal, tri_normal, cross_ab);

    if (tri_normal.Length() == 0 && cross_a.Dot3(cross_b) > 0 && cross_a.Dot3(cross_c) > 0)
    {
        return true;
    }
    return false;
}

void Triangle::insertIntoGrid(Grid *g, Matrix *m)
{
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
}