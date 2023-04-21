#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#include "bmp.h"
#include "linalg.h"
#include "poly.h"

int main(int argc, char** argv)
{
	vec3_t a = vec3_init(1.75, 0.75, 0);
	vec3_t b = vec3_init(2.5, 2, 0);
	vec3_t c = vec3_init(3.25, 0.75, 0);
	vec3_t o = vec3_scale(vec3_add(vec3_add(a, b), c), 0.3333333333);
	
	tri_t t = tri_init(a, b, c);
	vec3_t a_bary = tri_bary(t, a);
	vec3_t b_bary = tri_bary(t, b);
	vec3_t c_bary = tri_bary(t, c); 
	vec3_t o_bary = tri_bary(t, o);

	printf
	(
		"CENTROID: <%f, %f, %f>\nA: <%f, %f, %f>\nB: <%f, %f, %f>\nC: <%f, %f, %f>\n",
		o_bary.x, o_bary.y, o_bary.z,
		a_bary.x, a_bary.y, a_bary.z,
		b_bary.x, b_bary.y, b_bary.z,
		c_bary.x, c_bary.y, c_bary.z
	);

	int width = atoi(argv[1]);
	int height = atoi(argv[2]);
	double ratio = (double) width / (double) height;
	double X = 5;
	double Y = X / ratio;	
	double dp = X / (double) width;

	BMP_t* bmp = BMP_create(width, height, 3);
	
	double y = 0;
	for(int row = 0; row < height; row++)
	{
		double x = 0;
		for(int col = 0; col < width; col++)
		{
			vec3_t p = vec3_init(x, y, 0);
			vec3_t uvw = tri_bary(t, p);
			
			if
			(
				uvw.x >= 0 && uvw.x <= 1 &&
				uvw.y >= 0 && uvw.y <= 1 &&
				uvw.z >= 0 && uvw.z <= 1
			)
			{
				colour_t colour = { 255*uvw.x, 255*uvw.y, 255*uvw.z, 255};
				BMP_set_pixel(bmp, col, row, colour);
			}
	
			x += dp;
		}
		y += dp;
	}

	BMP_write(bmp, "triangle.bmp");

	return 0;
}
