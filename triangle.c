#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "bmp.h"
#include "linalg.h"

vec3_t vec3_unit(vec3_t a)
{ return vec3_scale(a, 1/vec3_norm(a)); }

typedef struct
{
	vec3_t a;
	vec3_t b;
	vec3_t c;
} tri_t;

tri_t tri_init(vec3_t a, vec3_t b, vec3_t c)
{
	tri_t t;
	t.a = a;
	t.b = b;
	t.c = c;
	return t;
}

bool tri_equal(tri_t a, tri_t b)
{
	return vec3_eq(a.a, b.a) && vec3_eq(a.b, b.b) && vec3_eq(a.c, b.c);
}

double mat2_det(mat2_t A)
{
	return A.data[0][0] * A.data[1][1] - A.data[1][0] * A.data[0][1];
}

vec3_t bary(tri_t t, vec3_t p)
{
	vec3_t v_0 = vec3_sub(t.b, t.a);
	vec3_t v_1 = vec3_sub(t.c, t.a);
	vec3_t v_2 = vec3_sub(p, t.a);

	// t_1 = v(v_0 * v_0) + w(v_1 * v_0) = v_2 * v_0
	// t_2 = v(v_0 * v_1) + w(v_1 * v_1) = v_2 * v_1
	// T = sans-solution row picture of t_1,t_2 system
	// T_x, T_y = T where x,y column = solution column
	
	mat2_t T = mat2_init
	(
		vec3_dot(v_0, v_0), vec3_dot(v_1, v_0),
		vec3_dot(v_0, v_1), vec3_dot(v_1, v_1)
	);

	mat2_t T_u = T;
	T_u.data[0][0] = vec3_dot(v_2, v_0); T_u.data[1][0] = vec3_dot(v_2, v_1);

	mat2_t T_v = T;
	T_v.data[0][1] = vec3_dot(v_2, v_0); T_v.data[1][1] = vec3_dot(v_2, v_1);
	
	double T_det = mat2_det(T);
	double T_u_det = mat2_det(T_u);
	double T_v_det = mat2_det(T_v);

	// We're ready to apply Cramer's law and calculate our barycentric coefficients
	
	double u = T_u_det / T_det;
	double v = T_v_det / T_det;
	double w = 1 - u - v;

	return vec3_init(u, v, w);
}

vec3_t unbary(tri_t t, vec3_t p)
{
	return vec3_add(vec3_add(vec3_scale(t.a, p.x), vec3_scale(t.b, p.y)), vec3_scale(t.c, p.z));
}


int main(int argc, char** argv)
{
	vec3_t a = vec3_init(1.75, 0.75, 0);
	vec3_t b = vec3_init(2.5, 2, 0);
	vec3_t c = vec3_init(3.25, 0.75, 0);
	vec3_t o = vec3_scale(vec3_add(vec3_add(a, b), c), 0.3333333333);
	
	tri_t t = tri_init(a, b, c);

	vec3_t a_bary = bary(t, a);
	vec3_t b_bary = bary(t, b);
	vec3_t c_bary = bary(t, c); 
	vec3_t o_bary = bary(t, o);

	printf
	(
		"CENTROID: <%f, %f, %f>\nA: <%f, %f, %f>\nB: <%f, %f, %f>\nC: <%f, %f, %f>\n",
		o_bary.x, o_bary.y, o_bary.z,
		a_bary.x, a_bary.y, a_bary.z,
		b_bary.x, b_bary.y, b_bary.z,
		c_bary.x, c_bary.y, c_bary.z
	);

	int width = atoi(argv[1]);
	int height = atoi(argv[2]);
	double ratio = (double) width / (double) height;
	double X = 5;
	double Y = X / ratio;	
	double dp = X / (double) width;

	BMP_t* bmp = BMP_create(width, height, 3);
	
	double y = 0;
	for(int row = 0; row < height; row++)
	{
		double x = 0;
		for(int col = 0; col < width; col++)
		{
			vec3_t p = vec3_init(x, y, 0);
			vec3_t uvw = bary(t, p);
			
			if
			(
				uvw.x >= 0 && uvw.x <= 1 &&
				uvw.y >= 0 && uvw.y <= 1 &&
				uvw.z >= 0 && uvw.z <= 1
			)
			{
				// printf("ROW: %d COL: %d\n<%f,%f,%f>\n", row, col, p.x, p.y, p.z);
				colour_t colour = { 255*uvw.x, 255*uvw.y, 255*uvw.z, 255};
				BMP_set_pixel(bmp, col, row, colour);
			}
	
			x += dp;
		}
		y += dp;
	}

	BMP_write(bmp, "triangle.bmp");

	return 0;
}
