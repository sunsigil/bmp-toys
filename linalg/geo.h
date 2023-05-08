#ifndef GEO_H
#define GEO_H

#include "linalg.h"

mat_t barycentric(mat_t tri, mat_t p);
mat_t inverse_barycentric(mat_t tri, mat_t p);
mat_t tri_AABB(mat_t tri);
int AABB_intersect(mat_t aabb_1, mat_t aabb_2);
double point_line_dist(mat_t p, mat_t a, mat_t b);

#endif
