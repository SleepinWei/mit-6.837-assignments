#include "object3d.h"
#include "boundingbox.h"
#include"grid.h"
#include "matrix.h"
#include "GL/freeglut.h"

Grid::Grid(BoundingBox *bb, int nx, int ny, int nz)
{
    arr = vector<vector<vector<bool>>>(nx, vector<vector<bool>>(ny, vector<bool>(nz)));
    this->bb = bb;
    this->nx = nx;
    this->ny = ny;
    this->nz = nz;
}

void renderCube(Vec3f pos_min, Vec3f pos_max)
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

    Vec4f diffuse {1,1,1,1};
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, (float *)&diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, (float *)&diffuse);

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
                if (arr[i][j][k])
                {
                    Vec3f pos;
                    Vec3f::Mult(pos, size, {i, j, k});
                    pos += min_pos;

                    renderCube(pos,pos + size);
                }
            }
        }
    }
}

bool Grid::intersect(const Ray &r, Hit &h, float tmin)
{
}