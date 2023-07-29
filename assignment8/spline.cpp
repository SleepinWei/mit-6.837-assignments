#include "spline.h"
#include "arg_parser.h"
#include "matrix.h"
#include "vectors.h"
#include <GL/freeglut.h>

void Curve::Paint(ArgParser *args)
{

    glLineWidth(3);
    glColor3f(0, 0, 1);
    glBegin(GL_LINES);
    for (int i = 0; i < num_vertices - 1; i++)
    {
        glVertex3f(vertices[i].x(), vertices[i].y(), vertices[i].z());
        glVertex3f(vertices[i + 1].x(), vertices[i + 1].y(), vertices[i + 1].z());
    }
    glEnd();

    glPointSize(5);
    glBegin(GL_POINTS);
    glColor3f(1.0f, 0.0f, 0.0f);
    for (int i = 0; i < num_vertices; i++)
    {
        glVertex3f(vertices[i].x(), vertices[i].y(), vertices[i].z());
    }
    glEnd();
    // render control points
}

void BezierCurve::Paint(ArgParser *args)
{
    Curve::Paint(args);

    int num = args->curve_tessellation;
    int control_points = Curve::num_vertices;

    glBegin(GL_LINES);
    glColor3f(0, 1, 0);
    int num_splines = (control_points - 4) / 3 + 1;
    for (int i = 0; i < num_splines; i++)
    {
        int base = 3 * i;
        for (int j = 0; j < num; j++)
        {
            float t = 1.0f * j / num;
            float t2 = t + 1.0f / num;
            Vec3f q = Q(t, base);
            glVertex3f(q.x(), q.y(), q.z());
            Vec3f q2 = Q(t2, base);
            glVertex3f(q2.x(), q2.y(), q2.z());
        }
    }
    glEnd();
}

void BSplineCurve::Paint(ArgParser *args)
{
    Curve::Paint(args);

    int num = args->curve_tessellation;
    int control_points = Curve::num_vertices;

    glBegin(GL_LINES);
    glColor3f(0, 1, 0);
    int num_splines = (control_points - 3);
    for (int i = 0; i < num_splines; i++)
    {
        int base = i;
        for (int j = 0; j < num; j++)
        {
            float t = 1.0f * j / num;
            float t2 = t + 1.0f / num;
            Vec3f q = Q(t, base);
            glVertex3f(q.x(), q.y(), q.z());
            Vec3f q2 = Q(t2, base);
            glVertex3f(q2.x(), q2.y(), q2.z());
        }
    }
    glEnd();
}

Vec3f BezierCurve::Q(float t, int base)
{
    Vec3f Q(0, 0, 0);
    Q = powf((1 - t), 3) * vertices[base] + 3 * powf(1 - t, 2) * t * vertices[base + 1] + 3 * powf(1 - t, 1) * powf(t, 2) * vertices[base + 2] + powf(t, 3) * vertices[base + 3];
    return Q;
}

Vec3f BSplineCurve::Q(float t, int base)
{
    Vec3f Q(0, 0, 0);
    Q = powf((1 - t), 3) / 6.0f * vertices[base] + (3 * powf(t, 3) - 6 * powf(t, 2) + 4) / 6.0f * vertices[base + 1] + (-3 * powf(t, 3) + 3 * powf(t, 2) + 3 * t + 1) / 6.0f * vertices[base + 2] + powf(t, 3) / 6.0f * vertices[base + 3];
    return Q;
}
void BSplineCurve::OutputBSpline(FILE *file)
{
    fprintf(file, "bspline\n num_vertices %d\n",num_vertices);
    for (int i = 0; i < num_vertices;i++){
        fprintf(file, "%f %f %f\n", vertices[i].x(), vertices[i].y(), vertices[i].z());
    }
}
void BSplineCurve::OutputBezier(FILE *file)
{
    fprintf(file, "bspline\n num_vertices 4\n");
    auto v1 = vertices[0];
    auto v2 = vertices[1];
    auto v3 = vertices[2];
    auto v4 = vertices[3];

    float g2[] = {
        v1.x(), v2.x(), v3.x(), v4.x(),
        v1.y(), v2.y(), v3.y(), v4.y(),
        v1.z(), v2.z(), v3.z(), v4.z(),
        0, 0, 0, 0};
    Matrix G2(g2);

    float b2[] = {
        -1, 3, -3, 1,
        3, -6, 0, 4,
        -3, 3, 3, 1,
        1, 0, 0, 0};
    Matrix B2(b2);
    B2 = 1.0f / 6 * B2;

    float b1[] = {
        -1, 3, -3, 1,
        3, -6, 3, 0,
        -3, 3, 0, 0,
        1, 0, 0, 0};
    Matrix B1(b1);

    B1.Inverse();

    Matrix G1 = G2 * B2 * B1;

    for (int i = 0; i < 4; i++)
    {
        fprintf(file, "%f %f %f\n", G1.Get(i, 0), G1.Get(i, 1), G1.Get(i, 2));
    }
    printf("\n");
}

void BezierCurve::OutputBSpline(FILE *file)
{
    fprintf(file, "bezier\n num_vertices 4\n");
    auto v1 = vertices[0];
    auto v2 = vertices[1];
    auto v3 = vertices[2];
    auto v4 = vertices[3];

    float g2[] = {
        v1.x(), v2.x(), v3.x(), v4.x(),
        v1.y(), v2.y(), v3.y(), v4.y(),
        v1.z(), v2.z(), v3.z(), v4.z(),
        0, 0, 0, 0};
    Matrix G1(g2);

    float b2[] = {
        -1, 3, -3, 1,
        3, -6, 0, 4,
        -3, 3, 3, 1,
        1, 0, 0, 0};

    float b1[] = {
        -1, 3, -3, 1,
        3, -6, 3, 0,
        -3, 3, 0, 0,
        1, 0, 0, 0};
    Matrix B1(b1);
    Matrix B2(b2);
    B2 = 1.0f / 6 * B2;

    B2.Inverse();

    Matrix result = G1 * B1 * B2;

    for (int i = 0; i < 4; i++)
    {
        fprintf(file, "%f %f %f\n", result.Get(i, 0), result.Get(i, 1), result.Get(i, 2));
    }
    printf("\n");
}
void BezierCurve::OutputBezier(FILE *file){
    fprintf(file, "bezier\n num_vertices %d\n",num_vertices);
    for (int i = 0; i < num_vertices;i++){
        fprintf(file, "%f %f %f\n", vertices[i].x(), vertices[i].y(), vertices[i].z());
    }
}

void Curve::moveControlPoint(int selectedPoint, float x, float y)
{
    vertices[selectedPoint] = {x, y, 0};
}
void Curve::addControlPoint(int selectedPoint, float x, float y)
{
    // vertices[selectedPoint]
    vertices.insert(vertices.begin() + selectedPoint, {x, y, 0});
    num_vertices += 1;
}
void Curve::deleteControlPoint(int selectedPoint)
{
    num_vertices -= 1;
    vertices.erase(vertices.begin() + selectedPoint);
}
TriangleMesh *SurfaceOfRevolution::OutputTriangles(ArgParser *args)
{
    int curve_tess = args->curve_tessellation;
    int revo_tess = args->revolution_tessellation;

    float angle = 2.0f * 3.1415f / revo_tess;
    Matrix matRotate = Matrix::MakeYRotation(angle);

    auto points = c->tessellation(curve_tess);

    auto *mesh = new TriangleNet(points.size() - 1, revo_tess);
    for (auto i = 0; i <= revo_tess; i++)
    {
        for (int j = 0; j < points.size(); j++)
        {
            mesh->SetVertex(j, i, points[j]);
            matRotate.Transform(points[j]);
        }
    }
    return mesh;
}

Vec3f CB(Vec3f p1, Vec3f p2, Vec3f p3, Vec3f p4, float t)
{
    Vec3f Q(0, 0, 0);
    Q = powf((1 - t), 3) * p1 + 3 * powf(1 - t, 2) * t * p2 + 3 * powf(1 - t, 1) * powf(t, 2) * p3 + powf(t, 3) * p4;
    return Q;
}

TriangleMesh *BezierPatch::OutputTriangles(ArgParser *args)
{
    int tess = args->patch_tessellation;

    TriangleNet *mesh = new TriangleNet(tess, tess);

    for (auto i = 0; i <= tess; i++)
    {
        for (int j = 0; j <= tess; j++)
        {
            float s = i * 1.0f / tess;
            float t = j * 1.0f / tess;
            Vec3f points[4];
            for (int k = 0; k < 4; k++)
            {
                points[k] = CB(vertices[4 * k], vertices[4 * k + 1], vertices[4 * k + 2], vertices[4 * k + 3], t);
            }
            Vec3f q = CB(points[0], points[1], points[2], points[3], s);
            mesh->SetVertex(j, i, q);
        }
    }
    return mesh;
}

void BezierPatch::Paint(ArgParser *args)
{

    glLineWidth(3);
    glColor3f(0, 0, 1);
    glBegin(GL_LINES);
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            int index = j + 4 * i;
            glVertex3f(vertices[index].x(), vertices[index].y(), vertices[index].z());
            glVertex3f(vertices[index + 1].x(), vertices[index + 1].y(), vertices[index + 1].z());
        }
    }
    glEnd();

    glPointSize(5);
    glBegin(GL_POINTS);
    glColor3f(1.0f, 0.0f, 0.0f);
    for (int i = 0; i < 16; i++)
    {
        glVertex3f(vertices[i].x(), vertices[i].y(), vertices[i].z());
    }
    glEnd();
}

void SurfaceOfRevolution::Paint(ArgParser *args)
{
    c->Paint(args);
}

vector<Vec3f> BezierCurve::tessellation(int tess)
{
    int num_splines = (vertices.size()- 4) / 3 + 1;
    vector<Vec3f> points((tess + 1)*num_splines);
    for (int s = 0; s < num_splines; s++)
    {
        for (int i = 0; i <= tess; i++)
        {
            float t = i * 1.0f / tess;
            points[(tess + 1)*s + i] = Q(t, 3 * s);
        }
    }
    return points;
}

vector<Vec3f> BSplineCurve::tessellation(int tess)
{
    int splines = vertices.size() - 3;
    vector<Vec3f> points((tess + 1) * splines);
    for (int s = 0; s < splines; s++)
    {
        for (int i = 0; i <= tess; i++)
        {
            float t = i * 1.0f / tess;
            points[(tess+1) * s + i] = Q(t, s);
        }
    }
    return points;
}