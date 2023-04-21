#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "bmp.h"
#include "bmp_mapped.h"

typedef struct window
{
	int x_i;
	int y_i;
	int x_f;
	int y_f;
} window_t;

typedef struct sample
{
	double lum_stdd;
	colour_t mean_col;
} sample_t;

double luminance(colour_t c)
{ return 0.2162 * c.r + 0.7152 * c.g + 0.0722 * c.b; }

window_t build_window(int x, int y, int a, int i)
{
	window_t w = {x, y, x, y};
	if(i == 0 || i == 3)
	{ w.x_f += a; }
	else
	{ w.x_i -= a; }
	if(i == 0 || i == 1)
	{ w.y_f += a; }
	else
	{ w.y_i -= a; }

	w.x_i = fmin(w.x_i, w.x_f);
	w.x_f = fmax(w.x_i, w.x_f);
	w.y_i = fmin(w.y_i, w.y_f);
	w.y_f = fmax(w.y_i, w.y_f);

	return w;
}

sample_t build_sample(BMP_mapped_t* bmpm, window_t w)
{
	int r_sum = 0; int g_sum = 0; int b_sum = 0;
	double lum_sum = 0;
	double lum_squared_sum = 0;
	int samples = 0;
	
	for(int y = w.y_i; y <= w.y_f; y++)
	{
		if(y < 0 || y >= bmpm->bmp.height)
		{ continue; }

		for(int x = w.x_i; x <= w.x_f; x++)
		{
			if(x < 0 || x >= bmpm->bmp.width)
			{ continue; }

			colour_t c = BMPM_get_pixel(bmpm, x, y);
			r_sum += c.r; g_sum += c.g; b_sum += c.b;

			double l = luminance(c);
			lum_sum += l;
			lum_squared_sum += l*l;

			samples++;
		}
	}

	double mean_lum = lum_sum / samples;
	double lum_stdd = lum_squared_sum / samples - mean_lum * mean_lum;
	colour_t mean_col = {r_sum/samples, g_sum/samples, b_sum/samples, 255};
	return (sample_t) {lum_stdd, mean_col};
}

int pick_sample(sample_t* samples, int n_samples)
{
	double min_stdd = INFINITY;
	int min_stdd_idx = -1;

	for(int i = 0; i < n_samples; i++)
	{
		sample_t s = samples[i];
		if(s.lum_stdd < min_stdd)
		{
			min_stdd = s.lum_stdd;
			min_stdd_idx = i;
		}
	}

	return min_stdd_idx;
}

int main(int argc, char** argv)
{
	if(argc < 4)
	{
		puts("[main] usage: kuwahara <window size> <BMP file IN> <BMP file OUT>");
		exit(EXIT_FAILURE);
	}

	BMP_mapped_t in_bmpm = BMPM_map(argv[2]);
	BMP_t out_bmp = BMP_create(in_bmpm.bmp.width, in_bmpm.bmp.height, 3);
	int window_size = atoi(argv[1]);

	for(int y = 0; y < in_bmpm.bmp.height; y++)
	{
		for(int x = 0; x < in_bmpm.bmp.width; x++)
		{
			sample_t samples[4];
			for(int i = 0; i < 4; i++)
			{
				window_t w = build_window(x, y, window_size, i);
				samples[i] = build_sample(&in_bmpm, w);
			}
			sample_t s = samples[pick_sample(samples, 4)];

			BMP_set_pixel(&out_bmp, x, y, s.mean_col);
		}
	}
	
	BMP_write(&out_bmp, argv[3]);
	BMPM_unmap(&in_bmpm);
	BMP_dispose(&out_bmp);

	return 0;
}
