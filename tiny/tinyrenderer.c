#include "bmp.h"
#include "obj.h"
#include "linalg.h"
#include "poly.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>

void nln(){ puts(""); }
void printn(double n){ printf("[N] %f\n", n); }
void print_vec3(vec3_t u)
{ printf("[%7.2f, %7.2f, %7.2f]\n", u.x, u.y, u.z); }
void print_vec4(vec4_t u)
{ printf("[%7.2f, %7.2f, %7.2f, %7.2f]\n", u.x, u.y, u.z, u.w); }
void print_mat4(mat4_t A)
{
	printf("[M]\n");
	print_vec4(mat4_get_row(A, 0));
	print_vec4(mat4_get_row(A, 1));
	print_vec4(mat4_get_row(A, 2));
	print_vec4(mat4_get_row(A, 3));
}

vec3_t barycentric(vec3_t a, vec3_t b, vec3_t c, vec3_t p)
{
	vec3_t v_0 = vec3_sub(b, a);
	vec3_t v_1 = vec3_sub(c, a);
	vec3_t v_2 = vec3_sub(p, a);

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

vec3_t surface_normal(vec4_t a, vec4_t b, vec4_t c)
{
	vec4_t ab = vec4_sub(b, a);
	vec4_t ac = vec4_sub(c, a);
	vec3_t abp = vec3_init(ab.x, ab.y, ab.z);
	vec3_t acp = vec3_init(ac.x, ac.y, ac.z);
	return vec3_unit(vec3_cross(abp, acp));
}

void line(int x0, int y0, int x1, int y1, BMP_t* bmp, colour_t c)
{
	// if the line is steep, transpose its start and end points
	bool steep = abs(y1-y0) > abs(x1-x0);
	if(steep)
	{
		int temp = x0;
		x0 = y0;
		y0 = temp;

		temp = x1;
		x1 = y1;
		y1 = temp;
	}

	// if the line heads left, swap its start and end points
	bool leftward = x0 > x1;
	if(leftward)
	{
		int temp = x0;
		x0 = x1;
		x1 = temp;

		temp = y0;
		y0 = y1;
		y1 = temp;
	}
	
	int dx = x1 - x0;
	int dy = y1 - y0;
	// approximate d_err as abs(dy) / (dx ~= 0.5)
	int d_err = abs(dy) * 2;
	// account for line heading up or down
	int y_step = (y1 > y0) ? 1 : -1;

	int y = y0;
	int err = 0;
	// if line is steep, we swap x,y in the draw call to undo our earlier transposition
	if(steep)
	{
		for(int x = x0; x < x1; x++)
		{
			if(y >= 0 && y < bmp->width && x >= 0 && x < bmp->height)
			{ BMP_set_pixel(bmp, y, x, c); }

			err += d_err;
			if(err > dx)
			{
				y += y_step;
				err -= dx*2;
			}
		}
	}
	// we need two for loops, one for each branch, to remove branching inside the loop
	else
	{
		for(int x = x0; x < x1; x++)
		{
			if(x >= 0 && x < bmp->width && y >= 0 && y < bmp->height)
			{ BMP_set_pixel(bmp, x, y, c); }

			err += d_err;
			if(err > dx)
			{
				y += y_step;
				err -= dx*2;
			}
		}
	}
}

void wireframe(vec4_t a, vec4_t b, vec4_t c, BMP_t* bmp, colour_t col)
{
	puts("lining");

	a = vec4_scale(a, 1/a.w);
	b = vec4_scale(b, 1/b.w);
	c = vec4_scale(c, 1/c.w);

	line(a.x, a.y, b.x, b.y, bmp, col);
	line(b.x, b.y, c.x, c.y, bmp, col);
	line(c.x, c.y, a.x, a.y, bmp, col);
}

void triangle(vec4_t a, vec4_t b, vec4_t c, BMP_t* bmp, colour_t col)
{
	puts("filling");

	vec3_t ap = vec3_init(a.x/a.w, a.y/a.w, a.z/a.w);
	vec3_t bp = vec3_init(b.x/b.w, b.y/b.w, b.z/b.w);
	vec3_t cp = vec3_init(c.x/c.w, c.y/c.w, c.z/c.w);

	double min_x = fmin(fmin(ap.x, bp.x), cp.x);
	double max_x = fmax(fmax(ap.x, bp.x), cp.x);
	double min_y = fmin(fmin(ap.y, bp.y), cp.y);
	double max_y = fmax(fmax(ap.y, bp.y), cp.y);

	printn(min_x);
	printn(max_x);
	printn(min_y);
	printn(max_y);

	for(int y = min_y; y < max_y; y++)
	{
		if(y < 0 || y >= bmp->height)
		{ continue; }

		for(int x = min_x; x < max_x; x++)
		{
			if(x < 0 || x >= bmp->width)
			{ continue; }

			vec3_t app = ap; vec3_t bpp = bp; vec3_t cpp = cp;
			app.z = 0; bpp.z = 0; cpp.z = 0;
			vec3_t p = barycentric(app, bpp, cpp, vec3_init(x,y,0));

			if(p.x >= 0 && p.x <= 1 && p.y >= 0 && p.y <= 1 && p.z >= 0 && p.z <= 1)
			{
				BMP_set_pixel(bmp, x, y, col);
			}
		}
	}
}

mat4_t projection(double c)
{
	return (mat4_t)
	{
		1,  0, 0,    0,
		0,  1, 0,    0,
		0,  0, 1,    0,
		0,  0, -1/c, 1
	};
}

mat4_t lookat(vec3_t eye, vec3_t center, vec3_t up)
{
	vec3_t z = vec3_unit(vec3_sub(center, eye));
	vec3_t x = vec3_unit(vec3_cross(up, z));
	vec3_t y = vec3_unit(vec3_cross(z, x));

	mat4_t Minv = (mat4_t)
	{
		x.x, x.y, x.z, 0,
		y.x, y.y, y.z, 0,
		z.x, z.y, z.z, 0,
		0,   0,   0,   1
	};

	mat4_t Tr = (mat4_t)
	{
		1, 0, 0, -eye.x,
		0, 1, 0, -eye.y,
		0, 0, 1, -eye.z,
		0, 0, 0, 1
	};

	return mat4_mul(Minv, Tr);
}

mat4_t viewport(vec2_t o, vec2_t d, int zres)
{
	return (mat4_t)
	{
		d.x*0.5, 0, 0,        o.x+d.x*0.5,
		0, d.y*0.5, 0,        o.y+d.y*0.5,
		0, 0,       zres*0.5, zres*0.5,
		0, 0,       0,        1
	};
}

int main(int argc, char** argv)
{
	colour_t white = {255, 255, 255, 255};
	colour_t red = {255, 0, 0, 255};

	OBJ_t* obj = OBJ_read(argv[1]);
	tri_t* tris = OBJ_tris(obj);
	printf("%d verts, %d faces\n", obj->vert_count, obj->face_count);

	int width = 1024;
	int height = width;
	BMP_t* bmp = BMP_create(width, height, 3);

	vec3_t eye = vec3_init(1, 1, 1);
	vec3_t center = vec3_init(0,0,0);
	vec3_t up = vec3_init(0,1,0);

	mat4_t look = lookat(eye, center, up);
	mat4_t view = viewport(vec2_init(width*0.125, height*0.125), vec2_init(width*0.75, height*0.75), 255);
	mat4_t proj = projection(vec3_norm(vec3_sub(eye, center)));
	puts("Lookat");
	print_mat4(look);
	puts("Viewport");
	print_mat4(view);
	puts("Projection");
	print_mat4(proj);

	mat4_t trans = mat4_id();
	trans = mat4_mul(look, trans);
	trans = mat4_mul(proj, trans);
	trans = mat4_mul(view, trans);

	puts("Final");
	print_mat4(trans);
	puts("");

	for(int i = 0; i < obj->face_count; i++)
	{
		tri_t tri = tris[i];
		vec4_t a = vec3_promote(tri.a, 1);
		vec4_t b = vec3_promote(tri.b, 1);
		vec4_t c = vec3_promote(tri.c, 1);
		print_vec4(a);
		print_vec4(b);
		print_vec4(c);
		puts("->");

		a = mv4_mul(trans, a);
		b = mv4_mul(trans, b);
		c = mv4_mul(trans, c);

		vec3_t light = vec3_unit(vec3_init(1,1,1));
		vec3_t surf = surface_normal(a, b, c);
		double ldot = -vec3_dot(light, surf);
		colour_t col = {ldot*255, ldot*255, ldot*255, 255};
		double vdot = vec3_dot(vec3_unit(vec3_sub(center, eye)), surf);

		/*if(vdot > 0)
		{
			triangle(a, b, c, bmp, col);
		}*/
		print_vec4(a);
		print_vec4(b);
		print_vec4(c);
		triangle(a, b, c, bmp, col);
		wireframe(a, b, c, bmp, red);
	}

	for(int i = 0; i < obj->face_count; i++)
	{
		tri_t tri = tris[i];
		vec4_t a = vec3_promote(tri.a, 1);
		vec4_t b = vec3_promote(tri.b, 1);
		vec4_t c = vec3_promote(tri.c, 1);
		a = mv4_mul(view, a);
		b = mv4_mul(view, b);
		c = mv4_mul(view, c);

		wireframe(a, b, c, bmp, red);
	}
	
	BMP_write(bmp, "tinyrenderer.bmp");

	BMP_dispose(bmp);
	OBJ_dispose(obj);
	free(tris);

	return 0;
}
