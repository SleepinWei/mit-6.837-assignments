#ifndef SPLINE_H
#define SPLINE_H
#include <stdio.h>
#include <vector>
using std::vector;
#include "triangle_mesh.h"
#include "vectors.h"
class ArgParser;

class Spline
{
public:
    // FOR VISUALIZATION
    virtual ~Spline() {}
    virtual void Paint(ArgParser *args) {}

    // FOR CONVERTING BETWEEN SPLINE TYPES
    virtual void OutputBezier(FILE *file) {}
    virtual void OutputBSpline(FILE *file) {}

    // FOR CONTROL POINT PICKING
    virtual int getNumVertices() { return 0; }
    virtual Vec3f getVertex(int i) { return {}; }

    // FOR EDITING OPERATIONS
    virtual void moveControlPoint(int selectedPoint, float x, float y) {}
    virtual void addControlPoint(int selectedPoint, float x, float y) {}
    virtual void deleteControlPoint(int selectedPoint) {}

    // FOR GENERATING TRIANGLES
    virtual TriangleMesh *OutputTriangles(ArgParser *args) { return nullptr; }
};

class Curve : public Spline
{
public:
    Curve(int num_vertices) : num_vertices(num_vertices) { vertices = vector<Vec3f>(num_vertices); }
    virtual ~Curve() {}
    virtual void Paint(ArgParser *args);
    void set(int i, Vec3f v) { vertices[i] = v; }
    virtual Vec3f Q(float t, int base) { return {}; };

    virtual void OutputBezier(FILE *file) {}
    virtual void OutputBSpline(FILE *file) {}

    virtual int getNumVertices() { return num_vertices; }
    virtual Vec3f getVertex(int i) { return vertices[i]; }

    virtual void moveControlPoint(int selectedPoint, float x, float y);
    virtual void addControlPoint(int selectedPoint, float x, float y);
    virtual void deleteControlPoint(int selectedPoint);
    virtual vector<Vec3f> tessellation(int tess) = 0;

public:
    int num_vertices;
    vector<Vec3f> vertices;
};

class BezierCurve : public Curve
{
public:
    BezierCurve(int num_vertices) : Curve(num_vertices) {}
    virtual ~BezierCurve() {}
    void Paint(ArgParser *args);

    virtual Vec3f Q(float t, int base) override;
    vector<Vec3f> tessellation(int tess);
    virtual void OutputBezier(FILE *file);
    virtual void OutputBSpline(FILE *file);
};

class BSplineCurve : public Curve
{
public:
    BSplineCurve(int num_vertices) : Curve(num_vertices) {}
    virtual ~BSplineCurve() {}
    void Paint(ArgParser *args);

    virtual Vec3f Q(float t, int base) override;
    vector<Vec3f> tessellation(int tess);
    virtual void OutputBezier(FILE *file);
    virtual void OutputBSpline(FILE *file);
};

class Surface : public Spline
{
public:
    virtual ~Surface() {}
    virtual void Paint(ArgParser *args) {}

public:
};

class BezierPatch : public Surface
{
public:
    virtual ~BezierPatch() {}
    void set(int index, Vec3f v) { vertices[index] = v; };
    virtual TriangleMesh *OutputTriangles(ArgParser *args);
    void Paint(ArgParser *args);

public:
    Vec3f vertices[16];
};

class SurfaceOfRevolution : public Surface
{
public:
    SurfaceOfRevolution(Curve *c) : c(c){};
    virtual ~SurfaceOfRevolution() {}
    virtual TriangleMesh *OutputTriangles(ArgParser *args);
    void Paint(ArgParser *args);
    virtual int getNumVertices() { return c->getNumVertices(); }
    virtual Vec3f getVertex(int i) { return c->getVertex(i); }
    virtual void moveControlPoint(int selectedPoint, float x, float y) { c->moveControlPoint(selectedPoint, x, y); }
    virtual void addControlPoint(int selectedPoint, float x, float y) { c->addControlPoint(selectedPoint, x, y); }
    virtual void deleteControlPoint(int selectedPoint) { c->deleteControlPoint(selectedPoint); }
    virtual void OutputBezier(FILE *file)
    {
        fprintf(file, "surface_of_revolution\n");
        c->OutputBezier(file);
    }
    virtual void OutputBSpline(FILE *file)
    {
        fprintf(file, "surface_of_revolution\n");
        c->OutputBSpline(file);
    }

public:
    Curve *c;
};
#endif // !SPLINE_H