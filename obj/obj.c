#include "obj.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>

#define MODE_N 0
#define MODE_V 1
#define MODE_VN 2
#define MODE_VT 3
#define MODE_F 4
#define MODE_C 5

typedef struct
{
	char* buffer;
	int length;
} substr_t;

substr_t substr_init(char* buffer, int length)
{
	substr_t s;
	s.buffer = buffer;
	s.length = length;
	return s;
}

bool substr_eq(substr_t a, substr_t b)
{
	if(a.length != b.length)
	{ return false; }

	int i = 0;
	while(i < a.length)
	{
		if(a.buffer[i] != b.buffer[i])
		{ return false; }
		i++;
	}
	return true;
}

typedef struct
{
	double* buffer;
	int size;
	int next_empty;
} dlist_t;

dlist_t* dlist_init(int size)
{
	dlist_t* l = malloc(sizeof(dlist_t));
	l->buffer = malloc(sizeof(double) * size);
	l->size = size;
	l->next_empty = 0;
	return l;
}

void dlist_add(dlist_t* l, double element)
{
	if(l->next_empty == l->size)
	{
		double* old_buf = l->buffer;

		l->buffer = malloc(sizeof(double) * l->size * 2);
		l->size = l->size * 2;
		
		for(int i = 0; i < l->next_empty; i++)
		{
			l->buffer[i] = old_buf[i];
		}

		free(old_buf);
	}

	l->buffer[l->next_empty] = element;
	l->next_empty++;
}

void dlist_dispose(dlist_t* l)
{
	free(l->buffer);
	free(l);
}

void parse_line(dlist_t* list, char* line)
{
	int start = 0;
	bool in = false;
	char current = '\0';

	while((current = line[start]) != '\n')
	{
		bool num =
		isdigit(line[start]) ||
		line[start] == '-' ||
		line[start] == '.' ||
		line[start] == 'e';
		
		if(in)
		{
			double d = atof(line+start);
			dlist_add(list, d);
			while(num)
			{ start++; }
			in = false;
		}
		else
		{
			while(!num)
			{ start++; }
			in = true;
		}
	}
}

OBJ_t* OBJ_read(char* path)
{
	FILE* file = fopen(path, "rb");
	
	if(file == NULL)
	{
		perror("[OBJ_read] Failed to open file");
		exit(EXIT_FAILURE);
	}
	
	fseek(file, 0, SEEK_END);
	int byte_count = ftell(file);
	fseek(file, 0, SEEK_SET);
	char* bytes = malloc(byte_count);
	fread(bytes, 1, byte_count, file);
	fclose(file);

	dlist_t* vs = dlist_init(128);
	dlist_t* vns = dlist_init(128);
	dlist_t* vts = dlist_init(128);
	dlist_t* fs = dlist_init(128);

	int line_idx = 0;
	for(int i = 0; i < byte_count; i++)
	{
		substr_t code = substr_init(bytes+i, 2);
		substr_t v = substr_init("v ", 2);
		substr_t vn = substr_init("vn", 2);
		substr_t vt = substr_init("vt", 2);
		substr_t f = substr_init("f ", 2);

		if(substr_eq(code, v))
		{ parse_line(vs, bytes+i); }
		else if(substr_eq(code, vn)) 
		{ parse_line(vns, bytes+i); }
		else if(substr_eq(code, vt))
		{ parse_line(vts, bytes+i); }
		else if(substr_eq(code, f))
		{ parse_line(fs, bytes+i); }
		
		while(bytes[i] != '\n')
		{ i++; }
		line_idx++;
	}

	double* verts = malloc(sizeof(double) * vs->next_empty);
	for(int i = 0; i < vs->next_empty; i++)
	{ verts[i] = vs->buffer[i]; }
	
	double* norms = malloc(sizeof(double) * vns->next_empty);
	for(int i = 0; i < vns->next_empty; i++)
	{ norms[i] = vns->buffer[i]; }
	
	double* uvs = malloc(sizeof(double) * vts->next_empty);
	for(int i = 0; i < vts->next_empty; i++)
	{ uvs[i] = vts->buffer[i]; }
	
	int* faces = malloc(sizeof(int) * fs->next_empty);
	for(int i = 0; i < fs->next_empty; i++)
	{ faces[i] = (int) fs->buffer[i]; }
	
	OBJ_t* obj = malloc(sizeof(OBJ_t));
	obj->verts = verts;
	obj->norms = norms;
	obj->uvs = uvs;
	obj->faces = faces;
	obj->vert_count = vs->next_empty / 3;
	obj->norm_count = vns->next_empty / 3;
	obj->uv_count = vts->next_empty / 3;
	obj->face_count = fs->next_empty / 9;

	dlist_dispose(vs);
	dlist_dispose(vns);
	dlist_dispose(vts);
	dlist_dispose(fs);

	return obj;
}

vec3_t OBJ_face_vert(OBJ_t* obj, int fidx, int vidx)
{
	fidx *= 9;
	vidx *= 3;
	int* face_start = obj->faces + fidx;
	int* vert_start = face_start + vidx;
	int offset = (*vert_start) - 1;
	double* start = obj->verts + (offset*3);	

	vec3_t v;
	v.x = start[0];
	v.y = start[1];
	v.z = start[2];

	return v;
}

tri_t OBJ_face_tri(OBJ_t* obj, int fidx)
{
	tri_t t;
	t.a = OBJ_face_vert(obj, fidx, 0);
	t.b = OBJ_face_vert(obj, fidx, 1);
	t.c = OBJ_face_vert(obj, fidx, 2);
	return t;
}

tri_t* OBJ_tris(OBJ_t* obj)
{
	tri_t* tris = malloc(sizeof(tri_t) * obj->face_count);
	for(int i = 0; i < obj->face_count; i++)
	{
		tris[i] = OBJ_face_tri(obj, i);
	}
	return tris;
}

void OBJ_dispose(OBJ_t* obj)
{
	free(obj->verts);
	free(obj->faces);
	free(obj);
}

