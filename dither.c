#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "bmp.h"

double luminance(colour_t c)
{ return 0.2162 * c.r + 0.7152 * c.g + 0.0722 * c.b; }

int main(int argc, char** argv)
{
	BMP_t* bmp = BMP_read(argv[1]);
	double values[bmp->width * bmp->height];
	BMP_t* result = BMP_create(bmp->width, bmp->height, bmp->channels);

	for(int y = 0; y < bmp->height; y++)
	{
		for(int x = 0; x < bmp->width; x++)
		{
			colour_t c = BMP_get_pixel(bmp, x, y);
			values[bmp->width * y + x] = luminance(c) / 255;
		}
	}

	for(int y = 0; y < bmp->height; y++)
	{
		for(int x = 0; x < bmp->width; x++)
		{
			double v_i = values[bmp->width * y + x];
			uint8_t p = v_i > 0.5 ? 1 : 0;
			double v_d = v_i - p;
			
			if((x+1) < bmp->width)
			{ values[bmp->width * y + (x+1)] += v_d * (7.0/16.0); }
			if((x-1) > 0 && (y+1) < bmp->height)
			{ values[bmp->width * (y+1) + (x-1)] += v_d * (3.0/16.0); }
			if((y+1) < bmp->height)
			{ values[bmp->width * (y+1) + x] += v_d * (5.0/16.0); }
			if((x+1) < bmp->width && (y+1) < bmp->height)
			{ values[bmp->width * (y+1) + (x+1)] += v_d * (1.0/16.0); }

			colour_t c = { p*255, p*255, p*255, 255 };
			BMP_set_pixel(result, x, y, c);
		}
	}
	
	char* write_path = malloc(strlen(argv[1]) + 64);
	sprintf(write_path, "dither_%s", argv[1]);
	BMP_write(result, write_path);
}
