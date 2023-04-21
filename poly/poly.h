#ifndef POLY
#define POLY

#include "linalg.h"

typedef struct
{
	vec3_t a;
	vec3_t b;
	vec3_t c;
} tri_t;

tri_t tri_init(vec3_t a, vec3_t b, vec3_t c);

bool tri_equal(tri_t a, tri_t b);
vec3_t tri_bary(tri_t t, vec3_t p);
vec3_t tri_norm(tri_t t);

#endif
