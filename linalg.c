#include <math.h>
#include "linalg.h"

vec2_t vec2_init(double x, double y)
{
	vec2_t u;
	u.x = x;
	u.y = y;
	return u;
}

vec3_t vec3_init(double x, double y, double z)
{
	vec3_t u;
	u.x = x;
	u.y = y;
	u.z = z;
	return u;
}

vec4_t vec4_init(double x, double y, double z, double w)
{
	vec4_t u;
	u.x = x;
	u.y = y;
	u.z = z;
	u.w = w;
	return u;
}

mat2_t mat2_init(double m11, double m12, double m21, double m22)
{
	mat2_t m;
	m.data[0][0] = m11; m.data[0][1] = m12;
	m.data[1][0] = m12; m.data[1][1] = m22;
	return m;
}

mat2_t mat2_row_init(vec2_t r1, vec2_t r2)
{
	mat2_t m;
	m.data[0][0] = r1.x; m.data[0][1] = r1.y;
	m.data[1][0] = r2.x; m.data[1][1] = r2.y;
	return m;
}

mat2_t mat2_col_init(vec2_t c1, vec2_t c2)
{
	mat2_t m;
	m.data[0][0] = c1.x; m.data[0][1] = c2.x;
	m.data[1][0] = c1.y; m.data[1][1] = c2.y;
	return m;
}

mat3_t mat3_init
(
	double m11, double m12, double m13,
	double m21, double m22, double m23,
	double m31, double m32, double m33
)
{
	mat3_t m;
	m.data[0][0] = m11; m.data[0][1] = m12; m.data[0][2] = m13;
	m.data[1][0] = m21; m.data[1][1] = m22; m.data[1][2] = m23;
	m.data[2][0] = m31; m.data[2][1] = m32; m.data[2][2] = m33;
	return m;
}

mat3_t mat3_row_init(vec3_t r1, vec3_t r2, vec3_t r3)
{
	mat3_t m;
	m.data[0][0] = r1.x; m.data[0][1] = r1.y; m.data[0][2] = r1.z;
	m.data[1][0] = r2.x; m.data[1][1] = r2.y; m.data[1][2] = r2.z;
	m.data[2][0] = r3.x; m.data[2][1] = r3.y; m.data[2][2] = r3.z;
	return m;
}

mat3_t mat3_col_init(vec3_t c1, vec3_t c2, vec3_t c3)
{
	mat3_t m;
	m.data[0][0] = c1.x; m.data[0][1] = c2.x; m.data[0][2] = c3.x;
	m.data[1][0] = c1.y; m.data[1][1] = c2.y; m.data[1][2] = c3.y;
	m.data[2][0] = c1.z; m.data[2][1] = c2.z; m.data[2][2] = c3.z;
	return m;
}

mat4_t mat4_init
(
	double m11, double m12, double m13, double m14,
	double m21, double m22, double m23, double m24,
	double m31, double m32, double m33, double m34,
	double m41, double m42, double m43, double m44
)
{
	mat4_t m;
	m.data[0][0] = m11; m.data[0][1] = m12; m.data[0][2] = m13; m.data[0][3] = m14;
	m.data[1][0] = m21; m.data[1][1] = m22; m.data[1][2] = m23; m.data[1][3] = m24;
	m.data[2][0] = m31; m.data[2][1] = m32; m.data[2][2] = m33; m.data[2][3] = m34;
	m.data[3][0] = m41; m.data[3][1] = m42; m.data[3][2] = m43; m.data[3][3] = m44;
	return m;
}

mat4_t mat4_row_init(vec4_t r1, vec4_t r2, vec4_t r3, vec4_t r4)
{
	mat4_t m;
	m.data[0][0] = r1.x; m.data[0][1] = r1.y; m.data[0][2] = r1.z; m.data[0][3] = r1.w;
	m.data[1][0] = r2.x; m.data[1][1] = r2.y; m.data[1][2] = r2.z; m.data[1][3] = r2.w;
	m.data[2][0] = r3.x; m.data[2][1] = r3.y; m.data[2][2] = r3.z; m.data[2][3] = r3.w;
	m.data[3][0] = r4.x; m.data[3][1] = r4.y; m.data[3][2] = r4.z; m.data[3][3] = r4.w;
	return m;
}

mat4_t mat4_col_init(vec4_t c1, vec4_t c2, vec4_t c3, vec4_t c4)
{
	mat4_t m;
	m.data[0][0] = c1.x; m.data[0][1] = c2.x; m.data[0][2] = c3.x; m.data[0][3] = c4.x;
	m.data[1][0] = c1.y; m.data[1][1] = c2.y; m.data[1][2] = c3.y; m.data[1][3] = c4.y;
	m.data[2][0] = c1.z; m.data[2][1] = c2.z; m.data[2][2] = c3.z; m.data[2][3] = c4.z;
	m.data[3][0] = c1.w; m.data[3][1] = c2.w; m.data[3][2] = c3.w; m.data[3][3] = c4.w;
	return m;
}

vec2_t mat2_get_row(mat2_t A, int i)
{
	return vec2_init(A.data[i][0], A.data[i][1]);
}

vec3_t mat3_get_row(mat3_t A, int i)
{
	return vec3_init(A.data[i][0], A.data[i][1], A.data[i][2]);
}

vec4_t mat4_get_row(mat4_t A, int i)
{
	return vec4_init(A.data[i][0], A.data[i][1], A.data[i][2], A.data[i][3]);
}

vec2_t mat2_get_col(mat2_t A, int i)
{
	return vec2_init(A.data[0][i], A.data[1][i]);
}

vec3_t mat3_get_col(mat3_t A, int i)
{
	return vec3_init(A.data[0][i], A.data[1][i], A.data[2][i]);
}

vec4_t mat4_get_col(mat4_t A, int i)
{
	return vec4_init(A.data[0][i], A.data[1][i], A.data[2][i], A.data[3][i]);
}

bool vec2_eq(vec2_t u, vec2_t v)
{
	return u.x == v.x && u.y == v.y;
}

bool vec3_eq(vec3_t u, vec3_t v)
{
	return u.x == v.x && u.y == v.y && u.z == v.z;
}

bool vec4_eq(vec4_t u, vec4_t v)
{
	return u.x == v.x && u.y == v.y && u.z == v.z && u.w == v.w;
}

double vec2_norm(vec2_t u)
{
	return sqrt(u.x * u.x + u.y * u.y); 
}

double vec3_norm(vec3_t u)
{
	return sqrt(u.x * u.x + u.y * u.y + u.z * u.z);
}

double vec4_norm(vec4_t u)
{
	return sqrt(u.x * u.x + u.y * u.y + u.z * u.z + u.w * u.w);
}

vec2_t vec2_scale(vec2_t u, double l)
{
	vec2_t v;
	v.x = l * u.x;
	v.y = l * u.y;
	return v;
}

vec3_t vec3_scale(vec3_t u, double l)
{
	vec3_t v;
	v.x = l * u.x;
	v.y = l * u.y;
	v.z = l * u.z;
	return v;
}

vec4_t vec4_scale(vec4_t u, double l)
{
	vec4_t v;
	v.x = l * u.x;
	v.y = l * u.y;
	v.z = l * u.z;
	v.w = l * u.w;
	return v;
}

vec2_t vec2_add(vec2_t u, vec2_t v)
{
	vec2_t w;
	w.x = u.x + v.x;
	w.y = u.y + v.y;
	return w;
}

vec3_t vec3_add(vec3_t u, vec3_t v)
{
	vec3_t w;
	w.x = u.x + v.x;
	w.y = u.y + v.y;
	w.z = u.z + v.z;
	return w;
}

vec4_t vec4_add(vec4_t u, vec4_t v)
{
	vec4_t w;
	w.x = u.x + v.x;
	w.y = u.y + v.y;
	w.z = u.z + v.z;
	w.w = u.w + v.w;
	return w;
}

vec2_t vec2_sub(vec2_t u, vec2_t v)
{
	vec2_t w;
	w.x = u.x - v.x;
	w.y = u.y - v.y;
	return w;
}

vec3_t vec3_sub(vec3_t u, vec3_t v)
{
	vec3_t w;
	w.x = u.x - v.x;
	w.y = u.y - v.y;
	w.z = u.z - v.z;
	return w;
}

vec4_t vec4_sub(vec4_t u, vec4_t v)
{
	vec4_t w;
	w.x = u.x - v.x;
	w.y = u.y - v.y;
	w.z = u.z - v.z;
	w.w = u.w - v.w;
	return w;
}

double vec2_dot(vec2_t u, vec2_t v)
{
	return u.x * v.x + u.y * v.y;
}

double vec3_dot(vec3_t u, vec3_t v)
{
	return u.x * v.x + u.y * v.y + u.z * v.z;
}

double vec4_dot(vec4_t u, vec4_t v)
{
	return u.x * v.x + u.y * v.y + u.z * v.z + u.w * v.w;
}

vec3_t cross(vec3_t u, vec3_t v)
{
	vec3_t w;
	w.x = u.y * v.z - u.z * v.y;
	w.y = u.z * v.x - u.x * v.z;
	w.z = u.x * v.y - u.y * v.x;
	return w;
}

mat2_t mat2_mul(mat2_t A, mat2_t B)
{
	mat2_t C;
	C.data[0][0] = vec2_dot(mat2_get_row(A, 0), mat2_get_col(B, 0));
	C.data[0][1] = vec2_dot(mat2_get_row(A, 0), mat2_get_col(B, 1));
	C.data[1][0] = vec2_dot(mat2_get_row(A, 1), mat2_get_col(B, 0));
	C.data[1][1] = vec2_dot(mat2_get_row(A, 1), mat2_get_col(B, 1));
	return C;
}

mat3_t mat3_mul(mat3_t A, mat3_t B)
{
	mat3_t C;
	C.data[0][0] = vec3_dot(mat3_get_row(A, 0), mat3_get_col(B, 0));
	C.data[0][1] = vec3_dot(mat3_get_row(A, 0), mat3_get_col(B, 1));
	C.data[0][2] = vec3_dot(mat3_get_row(A, 0), mat3_get_col(B, 2));
	C.data[1][0] = vec3_dot(mat3_get_row(A, 1), mat3_get_col(B, 0));
	C.data[1][1] = vec3_dot(mat3_get_row(A, 1), mat3_get_col(B, 1));
	C.data[1][2] = vec3_dot(mat3_get_row(A, 1), mat3_get_col(B, 2));
	C.data[2][0] = vec3_dot(mat3_get_row(A, 2), mat3_get_col(B, 0));
	C.data[2][1] = vec3_dot(mat3_get_row(A, 2), mat3_get_col(B, 1));
	C.data[2][2] = vec3_dot(mat3_get_row(A, 2), mat3_get_col(B, 2));
	return C;
}

mat4_t mat4_mul(mat4_t A, mat4_t B)
{
	mat4_t C;
	C.data[0][0] = vec4_dot(mat4_get_row(A, 0), mat4_get_col(B, 0));
	C.data[0][1] = vec4_dot(mat4_get_row(A, 0), mat4_get_col(B, 1));
	C.data[0][2] = vec4_dot(mat4_get_row(A, 0), mat4_get_col(B, 2));
	C.data[0][3] = vec4_dot(mat4_get_row(A, 0), mat4_get_col(B, 3));
	C.data[1][0] = vec4_dot(mat4_get_row(A, 1), mat4_get_col(B, 0));
	C.data[1][1] = vec4_dot(mat4_get_row(A, 1), mat4_get_col(B, 1));
	C.data[1][2] = vec4_dot(mat4_get_row(A, 1), mat4_get_col(B, 2));
	C.data[1][3] = vec4_dot(mat4_get_row(A, 1), mat4_get_col(B, 3));
	C.data[2][0] = vec4_dot(mat4_get_row(A, 2), mat4_get_col(B, 0));
	C.data[2][1] = vec4_dot(mat4_get_row(A, 2), mat4_get_col(B, 1));
	C.data[2][2] = vec4_dot(mat4_get_row(A, 2), mat4_get_col(B, 2));
	C.data[2][3] = vec4_dot(mat4_get_row(A, 2), mat4_get_col(B, 3));
	C.data[3][0] = vec4_dot(mat4_get_row(A, 3), mat4_get_col(B, 0));
	C.data[3][1] = vec4_dot(mat4_get_row(A, 3), mat4_get_col(B, 1));
	C.data[3][2] = vec4_dot(mat4_get_row(A, 3), mat4_get_col(B, 2));
	C.data[3][3] = vec4_dot(mat4_get_row(A, 3), mat4_get_col(B, 3));
	return C;
}

