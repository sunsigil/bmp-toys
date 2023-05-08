#ifndef DRAW_H
#define DRAW_H

#include "BMP.h"
#include "linalg.h"

void line(BMP_t* bmp, colour_t c, int x0, int y0, int x1, int y1);
void wire_tri(BMP_t* bmp, colour_t clr, mat_t tri);
void fill_tri(BMP_t* bmp, colour_t clr, mat_t tri);
void hello_tri(BMP_t* bmp, mat_t tri);
void wire_AABB(BMP_t* bmp, colour_t clr, mat_t AABB);

#endif
