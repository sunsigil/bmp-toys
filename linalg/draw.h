#ifndef DRAW_H
#define DRAW_H

#include "BMP.h"
#include "linalg.h"

void dot(BMP_t* bmp, colour_t clr, int x, int y, double r);
void line(BMP_t* bmp, colour_t clr, int x0, int y0, int x1, int y1);
void wire_tri(BMP_t* bmp, colour_t clr, mat_t tri);
void fill_tri(BMP_t* bmp, colour_t clr, mat_t tri);
void wire_AABB(BMP_t* bmp, colour_t clr, mat_t AABB);
void shade_tri
(
	BMP_t* frame, double* depthbuf,
	mat_t v, mat_t vt, mat_t vn,
	BMP_t* tex,
	mat_t view, mat_t light
);
void shade_depth(BMP_t* frame, double* buf);

#endif
