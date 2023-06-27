#include "obj.h"

#include "linalg.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

int is_obj_num(char c)
{
	return
	isdigit(c) ||
	c == '.' ||
	c == '-' ||
	c == 'e';
}

void parse_line(char* line, double* dest)
{
	int pos = 0;
	int vals = 0;

	while(line[pos] != '\n')
	{
		if(is_obj_num(line[pos]))
		{
			double d = atof(line+pos);
			dest[vals] = d;
			vals++;
			
			while(is_obj_num(line[pos]))
			{ pos++; }
		}
		else
		{ pos++; }
	}
}

OBJ_t OBJ_read(char* path)
{
	int fd = open(path, O_RDONLY, S_IRUSR);
	if(fd == -1)
	{
		perror("[OBJ_read] open");
		exit(EXIT_FAILURE);
	}
	
	uint32_t byte_count = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);
	uint8_t* bytes = mmap(NULL, byte_count, PROT_READ, MAP_PRIVATE, fd, SEEK_SET);
	if(bytes == NULL)
	{
		perror("[OBJ_read] mmap");
		exit(EXIT_FAILURE);
	}

	int v_count = 0;
	int vt_count = 0;
	int vn_count = 0;
	int f_count = 0;

	int pos = 0;
	while(pos < byte_count)
	{
		if(memcmp(bytes+pos, "v ", 2) == 0)
		{ v_count++; }
		else if(memcmp(bytes+pos, "vt ", 3) == 0)
		{ vt_count++; }
		else if(memcmp(bytes+pos, "vn ", 3) == 0)
		{ vn_count++; }
		else if(memcmp(bytes+pos, "f ", 2) == 0)
		{ f_count++; }
		
		while(pos < byte_count && bytes[pos] != '\n')
		{ pos++; }
		pos++;
	}

	double vs[v_count*3];
	int vs_read = 0;
	double vts[vt_count*2];
	int vts_read = 0;
	double vns[vn_count*3];
	int vns_read = 0;
	double fs[f_count*9];
	int fs_read = 0;
	
	pos = 0;
	while(pos < byte_count)
	{
		if(memcmp(bytes+pos, "v ", 2) == 0)
		{
			parse_line(bytes+pos, vs+vs_read);
			vs_read += 3;
		}
		else if(memcmp(bytes+pos, "vt ", 3) == 0)
		{
			parse_line(bytes+pos, vts+vts_read);
			vts_read += 2;
		}
		else if(memcmp(bytes+pos, "vn ", 3) == 0)
		{
			parse_line(bytes+pos, vns+vns_read);
			vns_read += 3;
		}
		else if(memcmp(bytes+pos, "f ", 2) == 0)
		{
			parse_line(bytes+pos, fs+fs_read);
			fs_read += 9;
		}
		
		while(pos < byte_count && bytes[pos] != '\n')
		{ pos++; }
		pos++;
	}
	
	OBJ_t obj;

	obj.vs = calloc(v_count, sizeof(double) * 3);
	memcpy(obj.vs, vs, sizeof(double) * 3 * v_count);
	obj.vts = calloc(vt_count, sizeof(double) * 2);
	memcpy(obj.vts, vts, sizeof(double) * 2 * vt_count);
	obj.vns = calloc(vn_count, sizeof(double) * 3);
	memcpy(obj.vns, vns, sizeof(double) * 3 * vn_count);
	obj.fs = calloc(f_count, sizeof(int) * 9);
	for(int i = 0; i < f_count * 9; i++)
	{ obj.fs[i] = ((int) fs[i]) - 1; }

	obj.v_count = v_count;
	obj.vt_count = vt_count;
	obj.vn_count = vn_count;
	obj.f_count = f_count;
	
	if(munmap(bytes, byte_count) == -1)
	{
		perror("[OBJ_read] munmap");
		exit(EXIT_FAILURE);
	}
	if(close(fd) == -1)
	{
		perror("[OBJ_read] close");
		exit(EXIT_FAILURE);
	}

	return obj;
}

void OBJ_extract_vs(OBJ_t obj, mat_t* vs)
{
	for(int i = 0; i < obj.v_count*3; i+=3)
	{ vs[i/3] = mat_row(4, obj.vs[i], obj.vs[i+1], obj.vs[i+2], 1.0); }
}

void OBJ_extract_vts(OBJ_t obj, mat_t* vts)
{
	for(int i = 0; i < obj.vt_count*2; i+=2)
	{ vts[i/2] = mat_row(2, obj.vts[i], obj.vts[i+1]); }
}

void OBJ_extract_vns(OBJ_t obj, mat_t* vns)
{
	for(int i = 0; i < obj.vn_count*3; i+=3)
	{ vns[i/3] = mat_row(4, obj.vns[i], obj.vns[i+1], obj.vns[i+2], 0.0); }
}

void OBJ_extract_fs(OBJ_t obj, mat_t* fs)
{
	for(int i = 0; i < obj.f_count*9; i+=9)
	{
		mat_t a = mat_row(3, (double) obj.fs[i+0], (double) obj.fs[i+1], (double) obj.fs[i+2]);
		mat_t b = mat_row(3, (double) obj.fs[i+3], (double) obj.fs[i+4], (double) obj.fs[i+5]);
		mat_t c = mat_row(3, (double) obj.fs[i+6], (double) obj.fs[i+7], (double) obj.fs[i+8]);
		mat_t f = mat_init(3, 3);
		f = mat_set_row(f, 0, a);
		f = mat_set_row(f, 1, b);
		f = mat_set_row(f, 2, c);
		fs[i/9] = f;
	}
}

void OBJ_interleave(OBJ_t obj, mat_t* vertices)
{
	mat_t vs[obj.v_count];
	mat_t vts[obj.vt_count];
	mat_t vns[obj.vn_count];
	mat_t fs[obj.f_count];
	OBJ_extract_vs(obj, vs);
	OBJ_extract_vts(obj, vts);
	OBJ_extract_vns(obj, vns);
	OBJ_extract_fs(obj, fs);

	int vert_idx = 0;

	for(int fidx = 0; fidx < obj.f_count; fidx++)
	{
		mat_t f = fs[fidx];

		mat_t a_indices = mat_get_row(f, 0);
		mat_t b_indices = mat_get_row(f, 1);
		mat_t c_indices = mat_get_row(f, 2);

		mat_t a_v = vs[(int) a_indices.data[0]];
		mat_t a_vt = vts[(int) a_indices.data[1]];
		mat_t a_vn = vns[(int) a_indices.data[2]];

		mat_t b_v = vs[(int) b_indices.data[0]];
		mat_t b_vt = vts[(int) b_indices.data[1]];
		mat_t b_vn = vns[(int) b_indices.data[2]];
		
		mat_t c_v = vs[(int) c_indices.data[0]];
		mat_t c_vt = vts[(int) c_indices.data[1]];
		mat_t c_vn = vns[(int) c_indices.data[2]];

		mat_t a = mat_init(3, 4);
		a = mat_blit(a, 0, 0, a_v);	
		a = mat_blit(a, 1, 0, a_vt);	
		a = mat_blit(a, 2, 0, a_vn);	
		
		mat_t b = mat_init(3, 4);
		b = mat_blit(b, 0, 0, b_v);	
		b = mat_blit(b, 1, 0, b_vt);	
		b = mat_blit(b, 2, 0, b_vn);	
		
		mat_t c = mat_init(3, 4);
		c = mat_blit(c, 0, 0, c_v);	
		c = mat_blit(c, 1, 0, c_vt);	
		c = mat_blit(c, 2, 0, c_vn);	

		vertices[vert_idx+0] = a;
		vertices[vert_idx+1] = b;
		vertices[vert_idx+2] = c;
		vert_idx += 3;
	}
}

void OBJ_collate(OBJ_t obj, mat_t* vertices)
{
	mat_t vs[obj.v_count];
	mat_t vts[obj.vt_count];
	mat_t vns[obj.vn_count];
	mat_t fs[obj.f_count];
	OBJ_extract_vs(obj, vs);
	OBJ_extract_vts(obj, vts);
	OBJ_extract_vns(obj, vns);
	OBJ_extract_fs(obj, fs);

	int vert_idx = 0;

	for(int fidx = 0; fidx < obj.f_count; fidx++)
	{
		mat_t f = fs[fidx];

		mat_t a_indices = mat_get_row(f, 0);
		mat_t b_indices = mat_get_row(f, 1);
		mat_t c_indices = mat_get_row(f, 2);

		mat_t a_v = vs[(int) a_indices.data[0]];
		mat_t b_v = vs[(int) b_indices.data[0]];
		mat_t c_v = vs[(int) c_indices.data[0]];
		
		mat_t a_vt = vts[(int) a_indices.data[1]];
		mat_t b_vt = vts[(int) b_indices.data[1]];
		mat_t c_vt = vts[(int) c_indices.data[1]];
		
		mat_t a_vn = vns[(int) a_indices.data[2]];
		mat_t b_vn = vns[(int) b_indices.data[2]];
		mat_t c_vn = vns[(int) c_indices.data[2]];

		mat_t v = mat_init(3, 4);
		v = mat_blit(v, 0, 0, a_v);	
		v = mat_blit(v, 1, 0, b_v);	
		v = mat_blit(v, 2, 0, c_v);	
		
		mat_t vt = mat_init(3, 4);
		vt = mat_blit(vt, 0, 0, a_vt);	
		vt = mat_blit(vt, 1, 0, b_vt);	
		vt = mat_blit(vt, 2, 0, c_vt);	
		
		mat_t vn = mat_init(3, 4);
		vn = mat_blit(vn, 0, 0, a_vn);	
		vn = mat_blit(vn, 1, 0, b_vn);	
		vn = mat_blit(vn, 2, 0, c_vn);	

		vertices[vert_idx+0] = v;
		vertices[vert_idx+1] = vt;
		vertices[vert_idx+2] = vn;
		vert_idx += 3;
	}
}

void OBJ_dispose(OBJ_t* obj)
{
	free(obj->vs);
	free(obj->vns);
	free(obj->vts);
	free(obj->fs);
}

