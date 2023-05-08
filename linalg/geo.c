#include "geo.h"
#include <stdlib.h>
#include <math.h>

mat_t barycentric(mat_t tri, mat_t p)
{
	// Real-Time Collision Detection, Christer Ericson
	mat_t a = mat_get_row(tri, 0);
	mat_t b = mat_get_row(tri, 1);
	mat_t c = mat_get_row(tri, 2);

	mat_t v0 = mat_sub(b, a);
	mat_t v1 = mat_sub(c, a);
	mat_t v2 = mat_sub(p, a);

	double d00 = mat_inner(v0, v0).data[0];
	double d01 = mat_inner(v0, v1).data[0];
	double d11 = mat_inner(v1, v1).data[0];
	double d20 = mat_inner(v2, v0).data[0];
	double d21 = mat_inner(v2, v1).data[0];
	double denom = (d00 * d11) - (d01 * d01);

	double v = ((d11 * d20) - (d01 * d21)) / denom;
	double w = ((d00 * d21) - (d01 * d20)) / denom;
	double u = 1.0 - v - w;
	mat_t p_bary = mat_row(3, u, v, w);

	return p_bary;
}

mat_t inverse_barycentric(mat_t tri, mat_t p)
{
	mat_t a = mat_get_row(tri, 0);
	mat_t b = mat_get_row(tri, 1);
	mat_t c = mat_get_row(tri, 2);

	mat_t ua = mat_scale(a, p.data[0]);
	mat_t vb = mat_scale(b, p.data[1]);
	mat_t wc = mat_scale(c, p.data[2]);

	return mat_add(mat_add(ua, vb), wc);
}

mat_t tri_AABB(mat_t tri)
{
	mat_t a = mat_get_row(tri, 0);
	mat_t b = mat_get_row(tri, 1);
	mat_t c = mat_get_row(tri, 2);
	
	
	double min_x = fmin(fmin(a.data[0], b.data[0]), c.data[0]);
	double min_y = fmin(fmin(a.data[1], b.data[1]), c.data[1]);
	double min_z = fmin(fmin(a.data[2], b.data[2]), c.data[2]);
	double max_x = fmax(fmax(a.data[0], b.data[0]), c.data[0]);
	double max_y = fmax(fmax(a.data[1], b.data[1]), c.data[1]);
	double max_z = fmax(fmax(a.data[2], b.data[2]), c.data[2]);


	double radius_x = (max_x - min_x) * 0.5;
	double radius_y = (max_y - min_y) * 0.5;
	double radius_z = (max_z - min_z) * 0.5;
	double center_x = min_x + radius_x;
	double center_y = min_y + radius_y;
	double center_z = min_z + radius_z;
	mat_t center = mat_row(3, center_x, center_y, center_z);
	mat_t radii = mat_row(3, radius_x, radius_y, radius_z);

	mat_t aabb = mat_init(2, 3);
	aabb = mat_set_row(aabb, 0, center);
	aabb = mat_set_row(aabb, 1, radii);
	
	return aabb;
}

int AABB_intersect(mat_t aabb_1, mat_t aabb_2)
{
	mat_t c_1 = mat_get_row(aabb_1, 0);
	mat_t c_2 = mat_get_row(aabb_2, 0);
	mat_t r_1 = mat_get_row(aabb_1, 1);
	mat_t r_2 = mat_get_row(aabb_2, 1);

	if(abs(c_1.data[0] - c_2.data[0]) > (r_1.data[0] + r_2.data[0]))
	{ return 0; }
	if(abs(c_1.data[1] - c_2.data[1]) > (r_1.data[1] + r_2.data[1]))
	{ return 0; }
	if(abs(c_1.data[2] - c_2.data[2]) > (r_1.data[2] + r_2.data[2]))
	{ return 0; }
	
	return 1;
}

double point_line_dist(mat_t p, mat_t a, mat_t b)
{
	mat_t ap = mat_sub(p, a);
	mat_t ab = mat_sub(b, a);

	double apap = mat_inner(ap, ap).data[0];
	double apab = mat_inner(ap, ab).data[0];
	double abab = mat_inner(ab, ab).data[0];

	return  sqrt(apap - apab * apab / abab);
}

