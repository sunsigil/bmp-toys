#include "linalg.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>

mat_t mat_init(int m, int n)
{
	if(m*n > 16)
	{
		puts("[mat_init] product of matrix dimensions (m x n) may not exceed 16");
		exit(EXIT_FAILURE);
	}

	mat_t A;
	A.m = m;
	A.n = n;
	memset(A.data, 0, 16 * sizeof(double));

	return A;
}

mat_t mat_row(int n, ...)
{
	mat_t R = mat_init(1, n);

	va_list args;
	va_start(args, n);
	for(int j = 0; j < n; j++)
	{
		R.data[j] = va_arg(args, double);
	}
	va_end(args);

	return R;
}

mat_t mat_col(int m, ...)
{
	mat_t C = mat_init(m, 1);
	
	va_list args;
	va_start(args, m);
	for(int i = 0; i < m; i++)
	{
		C.data[i] = va_arg(args, double);
	}
	va_end(args);

	return C;
}

mat_t mat_id(int m)
{
	mat_t A = mat_init(m, m);

	for(int i = 0; i < m*m; i+=m+1)
	{
		A.data[i] = 1;
	}

	return A;
}

mat_t mat_set_row(mat_t A, int i, mat_t R)
{
	if(i >= A.m)
	{
		puts("[mat_set_row] row index out of bounds");
		exit(EXIT_FAILURE);
	}
	if(R.m != 1 || R.n != A.n)
	{
		puts("[mat_get_row] row has incorrect dimensions");
		exit(EXIT_FAILURE);
	}

	for(int j = 0; j < A.n; j++)
	{
		A.data[i * A.n + j] = R.data[j];
	}
	
	return A;
}

mat_t mat_set_col(mat_t A, int j, mat_t C)
{
	if(j >= A.n)
	{
		puts("[mat_set_col] column index out of bounds");
		exit(EXIT_FAILURE);
	}
	if(C.m != A.m || C.n != 1)
	{
		puts("[mat_set_col] column has incorrect dimensions");
		exit(EXIT_FAILURE);
	}

	for(int i = 0; i < A.m; i++)
	{
		A.data[i * A.n + j] = C.data[i];
	}
	
	return A;
}

mat_t mat_get_row(mat_t A, int i)
{
	if(i >= A.m)
	{
		puts("[mat_get_row] row index out of bounds");
		exit(EXIT_FAILURE);
	}

	mat_t R = mat_init(1, A.n);

	for(int j = 0; j < A.n; j++)
	{
		R.data[j] = A.data[i * A.n + j];
	}
	
	return R;
}

mat_t mat_get_col(mat_t A, int j)
{
	if(j >= A.n)
	{
		puts("[mat_get_col] column index out of bounds");
		exit(EXIT_FAILURE);
	}

	mat_t C = mat_init(A.m, 1);

	for(int i = 0; i < A.m; i++)
	{
		C.data[i] = A.data[i * A.n + j];
	}

	return C;
}

mat_t mat_trans(mat_t A)
{
	mat_t AT = mat_init(A.n, A.m);

	for(int i = 0; i < A.m; i++)
	{
		for(int j = 0; j < A.n; j++)
		{
			AT.data[i * AT.n + j] = A.data[j * A.n + i];
		}
	}

	return AT;
}

double mat_norm(mat_t A)
{
	double sum = 0;
	for(int i = 0; i < A.m*A.n; i++)
	{
		sum += A.data[i] * A.data[i]; 
	}

	return sqrt(sum);
}

mat_t mat_scale(mat_t A, double l)
{
	for(int i = 0; i < A.m*A.n; i++)
	{ A.data[i] *= l; }

	return A;
}

mat_t mat_add(mat_t A, mat_t B)
{
	if(A.m != B.m || A.n != B.n)
	{
		puts("[mat_add] operand dimensions do not match");
		exit(EXIT_FAILURE);
	}

	for(int i = 0; i < A.m*A.n; i++)
	{ A.data[i] += B.data[i]; }

	return A;
}

mat_t mat_sub(mat_t A, mat_t B)
{
	if(A.m != B.m || A.n != B.n)
	{
		puts("[mat_sub] operand dimensions do not match");
		exit(EXIT_FAILURE);
	}

	for(int i = 0; i < A.m*A.n; i++)
	{ A.data[i] -= B.data[i]; }

	return A;
}

mat_t mat_mul(mat_t A, mat_t B)
{
	if(A.n != B.m)
	{
		puts("[mat_mul] operand dimensions do not allow multiplication");
		exit(EXIT_FAILURE);
	}

	mat_t C = mat_init(A.m, B.n);

	for(int i = 0; i < C.m; i++)
	{
		for(int j = 0; j < C.n; j++)
		{
			double sum = 0;
			for(int k = 0; k < A.n; k++)
			{
				sum += A.data[i * A.n + k] * B.data[k * B.m + j];
			}
			C.data[i * C.n + j] = sum;
		}
	}

	return C;
}

mat_t mat_inner(mat_t A, mat_t B)
{
	if(A.m != B.m || A.n != B.n)
	{
		puts("[mat_inner] operand dimensions do not match");
		exit(EXIT_FAILURE);
	}

	mat_t BT = mat_trans(B);
	mat_t C = mat_mul(A, BT);

	return C;
}

double mat2_det(mat_t A)
{
	if(A.m != 2 || A.n != 2)
	{
		puts("[mat2_det] operand is not 2x2");
		exit(EXIT_FAILURE);
	}

	return A.data[0] * A.data[3] - A.data[1] * A.data[2]; 
}

mat_t vec3_cross(mat_t A, mat_t B)
{
	if(A.m != 1 || A.n != 3 || B.m != 1 || B.n != 3)
	{
		puts("[vec3_cross] operand is not 1x3");
		exit(EXIT_FAILURE);
	}

	double Ax = A.data[0]; double Bx = B.data[0];
	double Ay = A.data[1]; double By = B.data[1];
	double Az = A.data[2]; double Bz = B.data[2];

	return mat_row(3, Ay*Bz - Az*By, -Ax*Bz + Az*Bx, Ax*By - Ay*Bx);
}

void mat_print(mat_t A)
{
	printf("Matrix %d x %d\n", A.m, A.n);
	for(int i = 0; i < A.m; i++)
	{
		printf("[");
		for(int j = 0; j < A.n; j++)
		{
			printf(" %7.2f", A.data[i * A.n + j]);
		}
		printf(" ]\n");
	}
	printf("\n");
}

void num_print(double n)
{
	printf("%7.2f\n", n);
}
