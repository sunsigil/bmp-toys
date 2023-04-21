#ifndef LINALG_H
#define LINALG_H

typedef struct mat
{
	int m;
	int n;
	double data[16];
} mat_t;

mat_t mat_init(int m, int n);
mat_t mat_col(int m, ...);
mat_t mat_row(int n, ...);
mat_t mat_id(int m);

mat_t mat_set_row(mat_t A, int i, mat_t R);
mat_t mat_set_col(mat_t A, int j, mat_t C);
mat_t mat_get_row(mat_t A, int i);
mat_t mat_get_col(mat_t A, int j);
mat_t mat_trans(mat_t A);
double mat_norm(mat_t A);

mat_t mat_scale(mat_t A, double l);
mat_t mat_add(mat_t A, mat_t B);
mat_t mat_sub(mat_t A, mat_t B);
mat_t mat_mul(mat_t A, mat_t B);
mat_t mat_inner(mat_t A, mat_t B);

double mat2_det(mat_t A);
mat_t vec3_cross(mat_t A, mat_t B);

void mat_print(mat_t A);
void num_print(double n);

#endif
