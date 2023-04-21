#include "geo.h"
#include <stdio.h>

mat_t barycentric(mat_t tri, mat_t p)
{
	// Real-Time Collision Detection, Christer Ericson
	mat_t a = mat_get_row(tri, 0);
	mat_t b = mat_get_row(tri, 1);
	mat_t c = mat_get_row(tri, 2);

	mat_t v0 = mat_sub(b, a);
	mat_t v1 = mat_sub(c, a);
	mat_t v2 = mat_sub(p, a);

	mat_print(v0);
	mat_print(v1);
	mat_print(v2);

	double d00 = mat_inner(v0, v0).data[0];
	double d01 = mat_inner(v0, v1).data[0];
	double d11 = mat_inner(v1, v1).data[0];
	double d20 = mat_inner(v2, v0).data[0];
	double d21 = mat_inner(v2, v1).data[0];

	double denom = d00 * d11 - d01 * d01;
	double v = (d11 * d20 - d01 * d21) / denom;
	double w = (d00 * d21 - d01 * d20) / denom;
	double u = 1. - v - w;

	printf("%f * %f - %f * %f | %f - %f | %f\n", d00,d11,d01,d01, d00*d11, d01*d01, d00*d11 - d01*d01);

	return mat_row(3, u, v, w);
}

mat_t surface_normal(mat_t tri)
{
	mat_t a = mat_get_row(tri, 0);
	mat_t b = mat_get_row(tri, 1);
	mat_t c = mat_get_row(tri, 2);
	mat_t ab = mat_sub(b, a);
	mat_t ac = mat_sub(c, a);
	
	return vec3_cross(ab, ac);
}

