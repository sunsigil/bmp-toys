#include <stdbool.h>

typedef struct
{
	double x;
	double y;
} vec2_t;

typedef struct
{
	double x;
	double y;
	double z;
} vec3_t;

typedef struct
{
	double x;
	double y;
	double z;
	double w;
} vec4_t;

typedef struct
{
	double data[2][2];
} mat2_t;

typedef struct
{
	double data[3][3];
} mat3_t;

typedef struct
{
	double data[4][4];
} mat4_t;

vec2_t vec2_init(double x, double y);
vec3_t vec3_init(double x, double y, double z);
vec4_t vec4_init(double x, double y, double z, double w);

mat2_t mat2_init(double m11, double m12, double m21, double m22);
mat2_t mat2_row_init(vec2_t r1, vec2_t r2);
mat2_t mat2_col_init(vec2_t c1, vec2_t c2);

mat3_t mat3_init
(
	double m11, double m12, double m13,
	double m21, double m22, double m23,
	double m31, double m32, double m33
);
mat3_t mat3_row_init(vec3_t r1, vec3_t r2, vec3_t r3);
mat3_t mat3_col_init(vec3_t c1, vec3_t c2, vec3_t c3);

mat4_t mat4_init
(
	double m11, double m12, double m13, double m14,
	double m21, double m22, double m23, double m24,
	double m31, double m32, double m33, double m34,
	double m41, double m42, double m43, double m44
);
mat4_t mat4_row_init(vec4_t r1, vec4_t r2, vec4_t r3, vec4_t r4);
mat4_t mat4_row_init(vec4_t c1, vec4_t c2, vec4_t c3, vec4_t c4);

vec2_t mat2_get_row(mat2_t A, int i);
vec3_t mat3_get_row(mat3_t A, int i);
vec4_t mat4_get_row(mat4_t A, int i);

vec2_t mat2_get_col(mat2_t A, int i);
vec3_t mat3_get_col(mat3_t A, int i);
vec4_t mat4_get_col(mat4_t A, int i);

bool vec2_eq(vec2_t u, vec2_t v);
bool vec3_eq(vec3_t u, vec3_t v);
bool vec4_eq(vec4_t u, vec4_t v);

double vec2_norm(vec2_t u);
double vec3_norm(vec3_t u);
double vec4_norm(vec4_t u);

vec2_t vec2_scale(vec2_t u, double l);
vec3_t vec3_scale(vec3_t u, double l);
vec4_t vec4_scale(vec4_t u, double l);

vec2_t vec2_add(vec2_t u, vec2_t v);
vec3_t vec3_add(vec3_t u, vec3_t v);
vec4_t vec4_add(vec4_t u, vec4_t v);

vec2_t vec2_sub(vec2_t u, vec2_t v);
vec3_t vec3_sub(vec3_t u, vec3_t v);
vec4_t vec4_sub(vec4_t u, vec4_t v);

double vec2_dot(vec2_t u, vec2_t v);
double vec3_dot(vec3_t u, vec3_t v);
double vec4_dot(vec4_t u, vec4_t v);

vec3_t cross(vec3_t u, vec3_t v);

mat2_t mat2_mul(mat2_t A, mat2_t B);
mat3_t mat3_mul(mat3_t A, mat3_t B);
mat4_t mat4_mul(mat4_t A, mat4_t B);

