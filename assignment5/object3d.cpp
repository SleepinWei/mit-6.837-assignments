#include"object3d.h"
#include"matrix.h"
#include"boundingbox.h"

Grid::Grid(BoundingBox *bb, int nx, int ny, int nz){
    arr = vector<vector<vector<bool>>>(nx, vector<vector<bool>>(ny, vector<bool>(nz)));
    this->bb = bb;
    this->nx = nx;
    this->ny = ny;
    this->nz = nz; 
}

