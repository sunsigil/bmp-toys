#ifndef OBJ_H
#define OBJ_H

#include "linalg.h"

typedef struct OBJ
{
	double* vs;
	double* vts;
	double* vns;
	int* fs;

	int v_count;
	int vt_count;
	int vn_count;
	int f_count;

} OBJ_t;

OBJ_t OBJ_read(char* path);
void OBJ_extract_vs(OBJ_t obj, mat_t* vs);
void OBJ_extract_vts(OBJ_t obj, mat_t* vts);
void OBJ_extract_vns(OBJ_t obj, mat_t* vns);
void OBJ_extract_fs(OBJ_t obj, mat_t* fs);
void OBJ_interleave(OBJ_t obj, mat_t* vertices);
void OBJ_collate(OBJ_t obj, mat_t* vertices);
void OBJ_dispose(OBJ_t* obj);

#endif
