#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include "bmp.h"

// THE BASICS

typedef struct
{
	double x;
	double y;
	double z;
} vec3_t;

vec3_t vec3_init(double x, double y, double z)
{
	vec3_t result;
	result.x = x;
	result.y = y;
	result.z = z;
	return result;
}

bool vec3_equal(vec3_t a, vec3_t b)
{
	return a.x == b.x && a.y == b.y && a.z == b.z;
}

double vec3_norm(vec3_t a)
{
	return sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
}

vec3_t vec3_scale(vec3_t a, double s)
{
	vec3_t result;
	result.x = a.x * s;
	result.y = a.y * s;
	result.z = a.z * s;
	return result;
}

vec3_t vec3_add(vec3_t a, vec3_t b)
{
	vec3_t result;
	result.x = a.x + b.x;
	result.y = a.y + b.y;
	result.z = a.z + b.z;
	return result;
}

vec3_t vec3_sub(vec3_t a, vec3_t b)
{
	vec3_t result;
	result.x = a.x - b.x;
	result.y = a.y - b.y;
	result.z = a.z - b.z;
	return result;
}

vec3_t vec3_unit(vec3_t a)
{ return vec3_scale(a, 1/vec3_norm(a)); }

double vec3_dot(vec3_t a, vec3_t b)
{ return a.x * b.x + a.y * b.y + a.z * b.z; }

vec3_t vec3_cross(vec3_t a, vec3_t b)
{
	vec3_t result;
	result.x = a.y * b.z - b.y * a.z;
	result.y = a.z * b.x - b.z * a.x;
	result.z = a.x * b.y - b.x * a.y;
	return result;
}

typedef struct
{
	vec3_t a;
	vec3_t b;
	vec3_t c;
} tri_t;

tri_t tri_init(vec3_t a, vec3_t b, vec3_t c)
{
	tri_t result;
	result.a = a;
	result.b = b;
	result.c = c;
	return result;
}

bool tri_equal(tri_t a, tri_t b)
{
	return vec3_equal(a.a, b.a) && vec3_equal(a.b, b.b) && vec3_equal(a.c, b.c);
}

// FUCK SHIT STARTS HERE

double mat2_det(double a, double b, double c, double d)
{ return a*d - c*b; }

vec3_t bary(tri_t t, vec3_t p)
{
	vec3_t v_0 = vec3_sub(t.b, t.a);
	vec3_t v_1 = vec3_sub(t.c, t.a);
	vec3_t v_2 = vec3_sub(p, t.a);

	// t_1 = v(v_0 * v_0) + w(v_1 * v_0) = v_2 * v_0
	// t_2 = v(v_0 * v_1) + w(v_1 * v_1) = v_2 * v_1
	
	double dot_00 = vec3_dot(v_0, v_0);
	double dot_10 = vec3_dot(v_1, v_0);
	double dot_20 = vec3_dot(v_2, v_0);
	double dot_01 = vec3_dot(v_0, v_1);
	double dot_11 = vec3_dot(v_1, v_1);
	double dot_21 = vec3_dot(v_2, v_1);

	// T = sans-solution row picture of t_1,t_2 system
	// T_x, T_y = T where x,y column = solution column

	double T_11 = dot_00;
	double T_12 = dot_10;
	double T_21 = dot_01;
	double T_22 = dot_11;
	
	double T_u_11 = dot_20;
	double T_u_12 = dot_10;
	double T_u_21 = dot_21;
	double T_u_22 = dot_11;

	double T_v_11 = dot_00;
	double T_v_12 = dot_20;
	double T_v_21 = dot_01;
	double T_v_22 = dot_21;

	double d_T = mat2_det(T_11, T_12, T_21, T_22);
	double d_T_u = mat2_det(T_u_11, T_u_12, T_u_21, T_u_22);
	double d_T_v = mat2_det(T_v_11, T_v_12, T_v_21, T_v_22);

	// We're ready to apply Cramer's law and calculate our barycentric coefficients
	
	double u = d_T_u / d_T;
	double v = d_T_v / d_T;
	double w = 1 - u - v;

	return vec3_init(u, v, w);
}


int main()
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

	/*printf
	(
		"CENTROID: <%f, %f, %f>\nA: <%f, %f, %f>\nB: <%f, %f, %f>\nC: <%f, %f, %f>\n",
		o_bary.x, o_bary.y, o_bary.z,
		a_bary.x, a_bary.y, a_bary.z,
		b_bary.x, b_bary.y, b_bary.z,
		c_bary.x, c_bary.y, c_bary.z
	);*/

	int width = 1920;
	int height = 1080;
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
