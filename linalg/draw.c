#include "draw.h"
#include "geo.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <float.h>

double clamp(double n, double m, double M)
{
	if(n < m)
	{ return m; }
	else if(n > M)
	{ return M; }
	return n;
}

mat_t clr2mat(colour_t c)
{
	double r = c.r / 255.0;
	double g = c.g / 255.0;
	double b = c.b / 255.0;
	double a = c.a / 255.0;
	return mat_row(4, r, g, b, a);
}

colour_t mat2clr(mat_t m)
{
	unsigned char r = m.data[0] * 255;
	unsigned char g = m.data[1] * 255;
	unsigned char b = m.data[2] * 255;
	unsigned char a = m.data[3] * 255;
	colour_t c = {r, g, b, a};
	return c;
}

colour_t sample(BMP_t* tex, mat_t uv)
{
	int x = uv.data[0] * (tex->width-1);
	int y = uv.data[1] * (tex->height-1);
	return BMP_get_pixel(tex, x, y);
}

void dot(BMP_t* bmp, colour_t clr, int x, int y, double r)
{
	mat_t cen = mat_row(2, (double) x, (double) y);

	for(int row = y-r; row < y+r; row++)
	{
		if(row < 0 || row >= bmp->height)
		{ continue; }

		for(int col = x-r; col < x+r; col++)
		{
			if(col < 0 || col >= bmp->width)
			{ continue; }

			mat_t pt = mat_row(2, (double) col, (double) row);
			mat_t ray = mat_sub(pt, cen);
			double dist = mat_norm(ray);

			if(dist <= r)
			{ BMP_set_pixel(bmp, col, row, clr); }
		}
	}
}

// implementation based on Dmitri Sokolov's
void line(BMP_t* bmp, colour_t clr, int x0, int y0, int x1, int y1)
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
			{ BMP_set_pixel(bmp, y, x, clr); }

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
			{ BMP_set_pixel(bmp, x, y, clr); }

			err += d_err;
			if(err > dx)
			{
				y += y_step;
				err -= dx*2;
			}
		}
	}
}

void wire_tri(BMP_t* bmp, colour_t clr, mat_t tri)
{
	mat_t a = mat_get_row(tri, 0);
	mat_t b = mat_get_row(tri, 1);
	mat_t c = mat_get_row(tri, 2);

	line(bmp, clr, a.data[0], a.data[1], b.data[0], b.data[1]);
	line(bmp, clr, b.data[0], b.data[1], c.data[0], c.data[1]);
	line(bmp, clr, c.data[0], c.data[1], a.data[0], a.data[1]);
}

void wire_AABB(BMP_t* bmp, colour_t clr, mat_t aabb)
{
	mat_t center = mat_get_row(aabb, 0);
	mat_t radii = mat_get_row(aabb, 1);

	double min_x = center.data[0] - radii.data[0];
	double max_x = center.data[0] + radii.data[0];
	double min_y = center.data[1] - radii.data[1];
	double max_y = center.data[1] + radii.data[1];

	line(bmp, clr, min_x, min_y, max_x, min_y);
	line(bmp, clr, min_x, max_y, max_x, max_y);
	line(bmp, clr, min_x, min_y, min_x, max_y);
	line(bmp, clr, max_x, min_y, max_x, max_y);
}

void shade_tri
(
	BMP_t* frame, double* depthbuf,
	mat_t v, mat_t vt, mat_t vn,
	BMP_t* tex,
	mat_t eye, mat_t light
)
{
	mat_t a = mat_get_row(v, 0);
	mat_t b = mat_get_row(v, 1);
	mat_t c = mat_get_row(v, 2);
	mat_t xs = mat_get_col(v, 0);
	mat_t ys = mat_get_col(v, 1);
	mat_t zs = mat_get_col(v, 2);
	v = mat_set_col(v, 2, mat_col(3, 0.0, 0.0, 0.0));

	double min_x = mat_min(xs);
	double max_x = mat_max(xs);
	double min_y = mat_min(ys);
	double max_y = mat_max(ys);

	for(int y = min_y; y < max_y; y++)
	{
		if(y < 0 || y >= frame->height)
		{ continue; }

		for(int x = min_x; x < max_x; x++)
		{
			if(x < 0 || x >= frame->width)
			{ continue; }

			mat_t pixel = mat_row(3, (double) x, (double) y, 0.0);
			mat_t bary = barycentric(v, pixel);
			
			if
			(
			 	mat_min(bary) >= 0.0 &&
				mat_max(bary) <= 1.0 &&
				mat_validate(bary)
			)
			{
				double z = mat_lincomb(zs, bary).data[0];
				if(z <= 0.0 || z >= 1.0)
				{ continue; }
				double buf_z = depthbuf[y * frame->width + x];

				if(z > buf_z)
				{
					mat_t uv = inverse_barycentric(vt, bary);
					mat_t norm = inverse_barycentric(vn, bary);

					mat_t ones = mat_row(4, 1.0, 1.0, 1.0, 1.0);
					double nl = mat_dot(norm, light);
					double direct_lum = clamp(nl, 0.0, 1.0);
					mat_t direct_clr_mat = mat_scale(ones, direct_lum); 
					mat_t ambient_clr_mat = mat_scale(ones, 0.25);
					mat_t diffuse_clr_mat = clr2mat(sample(tex, uv));
					mat_t lambert_clr_mat = mat_compwise(mat_add(ambient_clr_mat, direct_clr_mat), diffuse_clr_mat);
					colour_t lambert_clr = mat2clr(lambert_clr_mat);
					
					BMP_set_pixel(frame, x, y, lambert_clr);
					// BMP_set_pixel(frame, x, y, sample(tex, uv));
					depthbuf[y * frame->width + x] = z;

					/*double third = 1.0 / 3.0;
					mat_t thirds = mat_row(3, third, third, third);
					mat_t center = inverse_barycentric(v, thirds);
					mat_t normal = inverse_barycentric(vn, thirds);
					mat_t bristle = mat_add(center, mat_scale(normal, 20.0)); 
					colour_t red = { 255, 0, 0, 255 };
					line(frame, red, center.data[0], center.data[1], bristle.data[0], bristle.data[1]);*/
				}
			}
		}
	}
}
	
void shade_depth(BMP_t* frame, double* buf)
{
	double min = DBL_MAX;
	double max = -DBL_MAX;
	
	for(int i = 0; i < frame->width*frame->height; i++)
	{
		min = fmin(min, buf[i]);
		if(buf[i] < 1.0)
		{ max = fmax(max, buf[i]); }
	}

	for(int y = 0; y < frame->height; y++)
	{
		for(int x = 0; x < frame->width; x++)
		{
			double d = buf[y * frame->width + x];
			if(d < 1.0)
			{ d = (d - min) / (max - min); }

			mat_t mat = mat_row(4, d, d, d, 1.0);
			colour_t clr = mat2clr(mat);
			BMP_set_pixel(frame, x, y, clr);
		}
	}
}

