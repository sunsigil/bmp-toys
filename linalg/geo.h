#ifndef GEO_H
#define GEO_H

#include "linalg.h"

mat_t barycentric(mat_t tri, mat_t p);
mat_t surface_normal(mat_t tri);

#endif
