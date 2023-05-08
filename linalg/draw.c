#include "draw.h"
#include "geo.h"
#include <stdlib.h>
#include <math.h>

void line(BMP_t* bmp, colour_t c, int x0, int y0, int x1, int y1)
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

void wire_tri(BMP_t* bmp, colour_t clr, mat_t tri)
{
	mat_t a = mat_get_row(tri, 0);
	mat_t b = mat_get_row(tri, 1);
	mat_t c = mat_get_row(tri, 2);

	line(bmp, clr, a.data[0], a.data[1], b.data[0], b.data[1]);
	line(bmp, clr, b.data[0], b.data[1], c.data[0], c.data[1]);
	line(bmp, clr, c.data[0], c.data[1], a.data[0], a.data[1]);
}

void fill_tri(BMP_t* bmp, colour_t clr, mat_t tri) 
{
	mat_t a = mat_get_row(tri, 0);
	mat_t b = mat_get_row(tri, 1);
	mat_t c = mat_get_row(tri, 2);
	
	double min_x = fmin(fmin(a.data[0], b.data[0]), c.data[0]);
	double max_x = fmax(fmax(a.data[0], b.data[0]), c.data[0]);
	double min_y = fmin(fmin(a.data[1], b.data[1]), c.data[1]);
	double max_y = fmax(fmax(a.data[1], b.data[1]), c.data[1]);

	for(int y = min_y; y < max_y; y++)
	{
		if(y < 0 || y >= bmp->height)
		{ continue; }

		for(int x = min_x; x < max_x; x++)
		{
			if(x < 0 || x >= bmp->width)
			{ continue; }

			mat_t p = mat_row(3, (double) x, (double) y, 0);
			mat_t p_bary = barycentric(tri, p);
				
			if
			(
				p_bary.data[0] >= 0 && p_bary.data[0] <= 1 &&
				p_bary.data[1] >= 0 && p_bary.data[1] <= 1 &&
				p_bary.data[2] >= 0 && p_bary.data[2] <= 1
			)
			{
				BMP_set_pixel(bmp, x, y, clr);
			}
		}
	}
}

void hello_tri(BMP_t* bmp, mat_t tri) 
{
	mat_t a = mat_get_row(tri, 0);
	mat_t b = mat_get_row(tri, 1);
	mat_t c = mat_get_row(tri, 2);
	
	double min_x = fmin(fmin(a.data[0], b.data[0]), c.data[0]);
	double max_x = fmax(fmax(a.data[0], b.data[0]), c.data[0]);
	double min_y = fmin(fmin(a.data[1], b.data[1]), c.data[1]);
	double max_y = fmax(fmax(a.data[1], b.data[1]), c.data[1]);

	for(int y = min_y; y < max_y; y++)
	{
		if(y < 0 || y >= bmp->height)
		{ continue; }

		for(int x = min_x; x < max_x; x++)
		{
			if(x < 0 || x >= bmp->width)
			{ continue; }

			mat_t p = mat_row(3, (double) x, (double) y, 0);
			mat_t p_bary = barycentric(tri, p);
			colour_t clr = { 255*p_bary.data[0], 255*p_bary.data[1], 255*p_bary.data[2], 255 };
				
			if
			(
				p_bary.data[0] >= 0 && p_bary.data[0] <= 1 &&
				p_bary.data[1] >= 0 && p_bary.data[1] <= 1 &&
				p_bary.data[2] >= 0 && p_bary.data[2] <= 1
			)
			{
				BMP_set_pixel(bmp, x, y, clr);
			}
		}
	}
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

