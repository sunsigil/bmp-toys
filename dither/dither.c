#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "bmp.h"
#include "bmp_mapped.h"

double luminance(colour_t c)
{ return 0.2162 * c.r + 0.7152 * c.g + 0.0722 * c.b; }

int main(int argc, char** argv)
{
	if(argc < 3)
	{
		puts("[main] usage: dither <BMP file IN> <BMP file OUT>");
		exit(EXIT_FAILURE);
	}
	
	BMP_t in_bmp = BMPM_map(argv[1]);
	// BMP_t in_bmp = BMP_read(argv[1]);
	BMP_t out_bmp = BMP_create(in_bmp.width, in_bmp.height, in_bmp.channels);

	double* values = calloc(sizeof(double), in_bmp.width * in_bmp.height);
	for(int y = 0; y < in_bmp.height; y++)
	{
		for(int x = 0; x < in_bmp.width; x++)
		{
			// fprintf(stderr, "(%d, %d) ", x, y);
			colour_t c = BMPM_get_pixel(&in_bmp, x, y);
			values[in_bmp.width * y + x] = luminance(c) / 255.0;
		}
	}

	for(int y = 0; y < in_bmp.height; y++)
	{
		for(int x = 0; x < in_bmp.width; x++)
		{
			double v_i = values[in_bmp.width * y + x];
			uint8_t p = v_i > 0.5 ? 1 : 0;
			double v_d = v_i - p;
			
			if((x+1) < in_bmp.width)
			{ values[in_bmp.width * y + (x+1)] += v_d * (7.0/16.0); }
			if((x-1) > 0 && (y+1) < in_bmp.height)
			{ values[in_bmp.width * (y+1) + (x-1)] += v_d * (3.0/16.0); }
			if((y+1) < in_bmp.height)
			{ values[in_bmp.width * (y+1) + x] += v_d * (5.0/16.0); }
			if((x+1) < in_bmp.width && (y+1) < in_bmp.height)
			{ values[in_bmp.width * (y+1) + (x+1)] += v_d * (1.0/16.0); }

			colour_t c = { p*255, p*255, p*255, 255 };
			BMP_set_pixel(&out_bmp, x, y, c);
		}
	}
	
	// BMP_dispose(&in_bmp);
	BMPM_unmap(&in_bmp);
	BMP_write(&out_bmp, argv[2]);
	BMP_dispose(&out_bmp);

	return 0;
}
