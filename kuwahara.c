#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "bmp.h"

double luminance(colour_t c)
{ return 0.2162 * c.r + 0.7152 * c.g + 0.0722 * c.b; }

colour_t sample(BMP_t* bmp, int X, int Y, int a, int x, int y)
{
	double min_stdd = INFINITY;
	colour_t mean_c = { 0, 0, 0, 255 };

	for(int i = 0; i < 4; i++)
	{
		int r_sum = 0;
		int g_sum = 0;
		int b_sum = 0;
		double l_sum = 0;
		double l_squared_sum = 0;
		int samples = 0;

		int start_x = x;
		int start_y = y;
		int end_x = x;
		int end_y = y;
		
		if(i == 0 || i == 3)
		{ end_x += a; }
		else
		{ start_x -= a; }
		if(i == 0 || i == 1)
		{ end_y += a; }
		else
		{ start_y -= a; }
	
		for(int x1 = fmin(start_x, end_x); x1 <= fmax(start_x, end_x); x1++)
		{
			if(x1 < 0 || x1 >= X)
			{ continue; }

			for(int y1 = fmin(start_y, end_y); y1 <= fmax(start_y, end_y); y1++)
			{
				if(y1 < 0 || y1 >= Y)
				{ continue; }

				colour_t c = BMP_get_pixel(bmp, x1, y1);
				r_sum += c.r; g_sum += c.g; b_sum += c.b;

				double l = luminance(c);
				l_sum += l;
				l_squared_sum += l*l;

				samples++;
			}
		}
		
		colour_t q_mean_c = { r_sum/samples, g_sum/samples, b_sum/samples, 255 };
		float q_mean_l = l_sum / samples;
		float q_stdd = l_squared_sum / samples - q_mean_l * q_mean_l;
		if(q_stdd < min_stdd)
		{
			min_stdd = q_stdd;
			mean_c = q_mean_c;
		}
	}

	return mean_c;
}

int main(int argc, char** argv)
{
	BMP_t* bmp = BMP_read(argv[1]);
	
	int X = bmp->width;
	int Y = bmp->height;
	int a = 4;

	BMP_t* result = BMP_create(X,Y,3);

	for(int y = 0; y < Y; y++)
	{
		for(int x = 0; x < X; x++)
		{
			colour_t c = sample(bmp, X,Y,a, x,y);
			BMP_set_pixel(result, x, y, c);
		}
	}
	
	char* write_path = malloc(strlen(argv[1]) + 64);
	sprintf(write_path, "kuwahara_%s", argv[1]);
	BMP_write(result, write_path);
}
