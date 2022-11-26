#include <stdio.h>
#include <complex.h>
#include <time.h>
#include "bmp.h"

#define MAX_ITS 1000

int iterate(complex c)
{
	complex z = 0 + 0 * I;

	for(int i = 0; i < MAX_ITS; i++)
	{
		z = cpow(z, 2) + c;
		
		if(cabs(z) > 2)
		{
			return i;
		}
	}

	return -1;
}

int main()
{
	int width = 1000;
	int height = 1000;
	int r = 4;

	colour_t black = { 0, 0, 0, 255 };
	colour_t red = { 255, 0, 0, 255 };
	colour_t green = { 0, 255, 0, 255 };
	colour_t blue = { 0, 0, 255, 255 };
	
	time_t start = clock();
	BMP_t* bmp = BMP_create(width, height, 3);
	time_t end = clock();
	double cpu_time = (double) (end - start) / CLOCKS_PER_SEC;
	printf("[Create] CPU Time: %f\n", cpu_time);

	double total_iterate_time = 0;
	double total_write_time = 0;
	for(int x = 0; x < width; x++)
	{
		for(int y = 0; y < height; y++)
		{
			float x_norm = (float) x / (float) width;
			float y_norm = (float) y / (float) height;
			float x_cent = x_norm - 0.5;
			float y_cent = y_norm - 0.5;
			float a = x_cent * r;
			float b = y_cent * r;
			
			start = clock();
			int count = iterate(a + b * I);
			end = clock();
			total_iterate_time += (double) (end - start) / CLOCKS_PER_SEC;
			colour_t c = blue;

			if(count == -1)
			{ c = black; }
			else if(count % 2 == 0)
			{ c = red; }
			else if(count % 3 == 0)
			{ c = green; }
			
			start = clock();
			BMP_set_pixel(bmp, x, y, c);
			end = clock();
			total_write_time += (double) (end - start) / CLOCKS_PER_SEC;
		}
	}
	printf("[Generate]\nIterate Time: %f\nWrite Time: %f\n", total_iterate_time, total_write_time);

	start = clock();
	BMP_write(bmp, "mandelbrot.bmp");
	end = clock();
	cpu_time = (double) (end - start) / CLOCKS_PER_SEC;
	printf("[Save] CPU Time: %f\n", cpu_time);
}
