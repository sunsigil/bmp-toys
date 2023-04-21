#ifndef OBJ
#define OBJ

#include "linalg.h"
#include "poly.h"

typedef struct
{
	double* verts;
	double* norms;
	double* uvs;
	int* faces;

	int vert_count;
	int norm_count;
	int uv_count;
	int face_count;

} OBJ_t;

OBJ_t* OBJ_read(char* path);
vec3_t OBJ_face_vert(OBJ_t* obj, int fidx, int vidx);
tri_t OBJ_face_tri(OBJ_t* obj, int fidx);
tri_t* OBJ_tris(OBJ_t* obj);
void OBJ_dispose(OBJ_t* obj);

#endif
