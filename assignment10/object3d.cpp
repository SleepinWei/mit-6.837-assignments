#include "object3d.h"
#include "GL/freeglut.h"
#include "boundingbox.h"
#include "grid.h"
#include "hit.h"
#include "material.h"
#include "matrix.h"
#include "ray.h"
#include"raytracing_stats.h"

Grid::Grid(BoundingBox *bb, int nx, int ny, int nz)
    : arr(nx, vector<vector<vector<Transform *>>>(ny, vector<vector<Transform*>>(nz, vector<Transform*>(0, nullptr))))
{
    this->bb = bb;
    this->nx = nx;
    this->ny = ny;
    this->nz = nz;

    this->mat = new PhongMaterial(
        {1.0f, 1.0f, 1.0f},
        {0.0f, 0, 0},
        1,
        {0, 0, 0},
        {0, 0, 0},
        1);
}

Grid::~Grid()
{
    if (this->mat)
    {
        delete mat;
    }
    for (int i = 0; i < nx;i++){
        for (int j = 0; j < ny;j++){
            for (int k = 0; k < nz;k++){
                for(auto object: arr[i][j][k]){
                    delete object; 
                }
            }
        }
    }
}

void renderCube(Vec3f pos_min, Vec3f pos_max, Vec3f color)
{
    Vec3f size = pos_max - pos_min;
    Vec3f pos0 = pos_min;
    Vec3f pos1 = pos_min + Vec3f(size.x(), 0, 0);
    Vec3f pos2 = pos_min + Vec3f(size.x(), size.y(), 0);
    Vec3f pos3 = pos_min + Vec3f(0, size.y(), 0);
    Vec3f pos4 = pos_min + Vec3f(0, 0, size.z());
    Vec3f pos5 = pos_min + Vec3f(size.x(), 0, size.z());
    Vec3f pos6 = pos_min + Vec3f(size.x(), size.y(), size.z());
    Vec3f pos7 = pos_min + Vec3f(0, size.y(), size.z());

    Vec4f diffuse{1, 1, 1, 1};
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, (float *)&color);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, (float *)&color);

    glBegin(GL_QUADS);
    glNormal3f(0, -1, 0);
    glVertex3fv((float *)&pos0);
    glVertex3fv((float *)&pos1);
    glVertex3fv((float *)&pos5);
    glVertex3fv((float *)&pos4);

    glNormal3f(1, 0, 0);
    glVertex3fv((float *)&pos1);
    glVertex3fv((float *)&pos2);
    glVertex3fv((float *)&pos6);
    glVertex3fv((float *)&pos5);

    glNormal3f(0, 0, 1);
    glVertex3fv((float *)&pos4);
    glVertex3fv((float *)&pos5);
    glVertex3fv((float *)&pos6);
    glVertex3fv((float *)&pos7);

    glNormal3f(0, 0, -1);
    glVertex3fv((float *)&pos0);
    glVertex3fv((float *)&pos3);
    glVertex3fv((float *)&pos2);
    glVertex3fv((float *)&pos1);

    glNormal3f(-1, 0, 0);
    glVertex3fv((float *)&pos0);
    glVertex3fv((float *)&pos4);
    glVertex3fv((float *)&pos7);
    glVertex3fv((float *)&pos3);

    glNormal3f(0, 1, 0);
    glVertex3fv((float *)&pos2);
    glVertex3fv((float *)&pos3);
    glVertex3fv((float *)&pos7);
    glVertex3fv((float *)&pos6);

    glEnd();
}

Vec3f mapNumToColor(int num,float scale =25.0f)
{
    Vec3f color(1.0f, 1.0f, 1.0f);
    color = color * (1.0f - num / scale * 0.8f);
    return color;
}

void Grid::paint()
{

    Vec3f max_pos = bb->getMax();
    Vec3f min_pos = bb->getMin();
    Vec3f size = max_pos - min_pos;
    size.Divide(nx, ny, nz);

    for (int i = 0; i < nx; i++)
    {
        for (int j = 0; j < ny; j++)
        {
            for (int k = 0; k < nz; k++)
            {
                int num = arr[i][j][k].size();
                if (num)
                {
                    Vec3f pos;
                    Vec3f::Mult(pos, size, {i, j, k});
                    pos += min_pos;

                    renderCube(pos, pos + size, mapNumToColor(num-1,scale));
                }
            }
        }
    }
}

bool Grid::intersect(const Ray &r, Hit &h, float tmin)
{
    Vec3f size = bb->getMax() - bb->getMin();
    size.Divide(nx, ny, nz);
    // bounding box intersection
    info = {};
    initializeRayMarch(info, r, 0.0f);
    if (info.tmin >= 1e5)
    {
        return false;
    }
    while (info.i >= 0 && info.i < nx && info.j >= 0 && info.j < ny && info.k >= 0 && info.k < nz)
    {
        int num = arr[info.i][info.j][info.k].size();
        if (num)
        {
            // std::cout << info.i << ' ' << info.j << ' ' << info.k << '\n';

            this->mat->setDiffuseColor(mapNumToColor(num-1,scale));

            h.set(info.tmin, this->mat, info.normal, r);
            return true;
        }
        info.nextCell();
    }
    return false;
}

void rayPlaneIntersection(const Ray &r, BoundingBox *bb, float &tmin, float &tmax, int index)
{
    int ix = index;
    Vec3f o = r.getOrigin();
    Vec3f d = r.getDirection();
    Vec3f pos_min = bb->getMin();
    Vec3f pos_max = bb->getMax();

    float tx0 = (pos_min[ix] - o[ix]) / d[ix];
    float tx1 = (pos_max[ix] - o[ix]) / d[ix];
    if (tx0 > tx1)
    {
        std::swap(tx0, tx1);
    }
    tmin = tx0;
    tmax = tx1;
}

bool rayBoxIntersection(const Ray &r, BoundingBox *bb, float &t)
{
    Vec3f pos_min = bb->getMin();
    Vec3f pos_max = bb->getMax();
    Vec3f size = pos_max - pos_min;

    Vec3f o = r.getOrigin();
    Vec3f d = r.getDirection();

    // o + t * d = p
    // x
    float tminx, tmaxx, tminy, tmaxy, tminz, tmaxz;
    rayPlaneIntersection(r, bb, tminx, tmaxx, 0);
    rayPlaneIntersection(r, bb, tminy, tmaxy, 1);
    rayPlaneIntersection(r, bb, tminz, tmaxz, 2);

    tminx = max(tminz, max(tminx, tminy));
    tmaxx = std::min(tmaxz, std::min(tmaxy, tmaxx));
    if (tmaxx <= tminx)
    {
        return false;
    }

    t = tminx;
    return true;
}

bool isInsideBoundingBox(Vec3f origin, BoundingBox *bb)
{
    Vec3f pos_min = bb->getMin();
    Vec3f pos_max = bb->getMax();
    return origin.x() <= pos_max.x() && origin.x() >= pos_min.x() && origin.y() <= pos_max.y() && origin.y() >= pos_min.y() && origin.z() >= pos_min.z() && origin.z() <= pos_max.z();
}

void Grid::initializeRayMarch(MarchingInfo &mi, const Ray &r, float tmin) const
{
    Vec3f pos_min = bb->getMin();
    Vec3f pos_max = bb->getMax();
    Vec3f size = pos_max - pos_min;
    size.Divide(nx, ny, nz);

    mi.sign_x = r.getDirection().x() > 0 ? 1 : -1;
    mi.sign_y = r.getDirection().y() > 0 ? 1 : -1;
    mi.sign_z = r.getDirection().z() > 0 ? 1 : -1;

    Vec3f d = r.getDirection();
    mi.d_tx = fabs(size.x() / d.x());
    mi.d_ty = fabs(size.y() / d.y());
    mi.d_tz = fabs(size.z() / d.z());

    mi.tmin = 1e6;
    // origin inside grid;
    if (isInsideBoundingBox(r.getOrigin(), bb))
    {
        Vec3f o = r.getOrigin();
        Vec3f grid_pos = o - pos_min;
        grid_pos.Divide(size.x(), size.y(), size.z());

        Vec3f pos = o;
        mi.tmin = 0;
        mi.i = (grid_pos.x());
        mi.j = (grid_pos.y());
        mi.k = (grid_pos.z());

        if (mi.i == nx)
            mi.i -= 1;
        if (mi.j == ny)
            mi.j -= 1;
        if (mi.k == nz)
            mi.k -= 1;

        int nexti = mi.i, nextj = mi.j, nextk = mi.k;
        if (mi.sign_x > 0)
        {
            nexti += 1;
        }
        if (mi.sign_y > 0)
        {
            nextj += 1;
        }
        if (mi.sign_z > 0)
        {
            nextk += 1;
        }

        Vec3f next_grid_delta = Vec3f{nexti,nextj,nextk} * size + pos_min - o;

        mi.t_next_x = next_grid_delta.x() / d.x();
        mi.t_next_y = next_grid_delta.y() / d.y();
        mi.t_next_z = next_grid_delta.z() / d.z();

        if(arr[mi.i][mi.j][mi.k].size()){
            // 已经有东西了，则往前进一步, 防止算阴影时一直被遮挡
            mi.nextCell();
        }
    }
    else
    {
        // test intersection
        float t = 1e6;
        bool isIntersect = rayBoxIntersection(r, bb, t);
        if (isIntersect)
        {
            Vec3f o = r.getOrigin();
            Vec3f d = r.getDirection();
            Vec3f pos = o + d * t;
            Vec3f grid_pos = pos - pos_min;
            grid_pos.Divide(size.x(), size.y(), size.z());

            mi.tmin = t;
            float i = grid_pos.x(), j = grid_pos.y(), k = grid_pos.z();
            // cout << i << ' ' << j << ' ' << k << '\n';
            // mi.i = fabs(round(i) - i) < 1e-6 ? round(i): i;
            // mi.j =  fabs(round(j) - j) < 1e-6 ? round(j): j;
            // mi.k =  fabs(round(k) - k) < 1e-6 ? round(k): k;
            mi.i = i, mi.j = j, mi.k = k;

            const float eps = 1e-6;

            if (pos.x() <= pos_min.x() + eps)
            {
                mi.normal = {-1, 0, 0};
            }
            else if (pos.x() >= pos_max.x() - eps)
            {
                mi.normal = {1, 0, 0};
            }
            else if (pos.y() <= pos_min.x() + eps)
            {
                mi.normal = {0, -1, 0};
            }
            else if (pos.y() >= pos_max.y() - eps)
            {
                mi.normal = {0, 1, 0};
            }
            else if (pos.z() <= pos_min.z() + eps)
            {
                mi.normal = {0, 0, -1};
            }
            else if (pos.z() >= pos_max.z() - eps)
            {
                mi.normal = {0, 0, 1};
            }
            else
            {
                // cout << "Wrong normal"<< pos.x() << ' ' << pos.y() << ' ' << pos.z() << "\n";
            }
            if (mi.i == nx)
                mi.i -= 1;
            if (mi.j == ny)
                mi.j -= 1;
            if (mi.k == nz)
                mi.k -= 1;

            int nexti = mi.i, nextj = mi.j, nextk = mi.k;
            if (mi.sign_x > 0)
            {
                nexti += 1;
            }
            if (mi.sign_y > 0)
            {
                nextj += 1;
            }
            if (mi.sign_z > 0)
            {
                nextk += 1;
            }

            Vec3f next_grid_delta = Vec3f{nexti, nextj, nextk} * size + pos_min - r.getOrigin();

            mi.t_next_x = next_grid_delta.x() / d.x();
            mi.t_next_y = next_grid_delta.y() / d.y();
            mi.t_next_z = next_grid_delta.z() / d.z();
        }
        else
        {
            mi.tmin = 1e6;
            return;
        }
    }
}


void MarchingInfo::nextCell()
{
    RayTracingStats::IncrementNumGridCellsTraversed();
    if (t_next_x < t_next_y && t_next_x < t_next_z)
    {
        // x 最小
        i = i + sign_x;
        tmin = t_next_x;
        t_next_x += d_tx;
        this->normal = {-sign_x, 0, 0};
    }
    else if (t_next_y < t_next_x && t_next_y < t_next_z)
    {
        // y 最小
        j = j + sign_y;
        tmin = t_next_y;
        t_next_y += d_ty;
        this->normal = {0, -sign_y, 0};
    }
    else
    {
        // z 最小
        k = k + sign_z;
        tmin = t_next_z;
        t_next_z += d_tz;
        this->normal = {0, 0, -sign_z};
    }
}