#include "poly.h"

#include <math.h>

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

vec3_t tri_bary(tri_t t, vec3_t p)
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

vec3_t tri_norm(tri_t t)
{
	vec3_t arm_b = vec3_sub(t.b, t.a);
	vec3_t arm_c = vec3_sub(t.c, t.a);
	return vec3_unit(vec3_cross(arm_b, arm_c));
}

