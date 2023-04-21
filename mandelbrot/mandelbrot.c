#include <stdio.h>
#include <complex.h>
#include <time.h>
#include <stdlib.h>
#include "bmp.h"

int iterate(complex c, int its)
{
	complex z = 0 + 0 * I;

	for(int i = 0; i < its; i++)
	{
		z = cpow(z, 2) + c;
		
		if(cabs(z) > 2)
		{
			return i;
		}
	}

	return -1;
}

int main(int argc, char** argv)
{
	if(argc < 4)
	{
		puts("[main] usage: mandelbrot <width> <height> <BMP file OUT>");
		exit(EXIT_FAILURE);
	}

	int width = atoi(argv[1]);
	int height = atoi(argv[2]);
	int r = 4;

	colour_t black = { 0, 0, 0, 255 };
	colour_t red = { 255, 0, 0, 255 };
	colour_t green = { 0, 255, 0, 255 };
	colour_t blue = { 0, 0, 255, 255 };
	
	BMP_t bmp = BMP_create(width, height, 3);

	for(int y = 0; y < height; y++)
	{
		for(int x = 0; x < width; x++)
		{
			float x_norm = (float) x / (float) width;
			float y_norm = (float) y / (float) height;
			float x_cent = x_norm - 0.5;
			float y_cent = y_norm - 0.5;
			float a = x_cent * r;
			float b = y_cent * r;
			
			int count = iterate(a + b * I, 10000);
			colour_t c = blue;

			if(count == -1)
			{ c = black; }
			else if(count % 2 == 0)
			{ c = red; }
			else if(count % 3 == 0)
			{ c = green; }
			
			BMP_set_pixel(&bmp, x, y, c);
		}
	}

	BMP_write(&bmp, argv[3]);
	BMP_dispose(&bmp);

	return 0;
}
