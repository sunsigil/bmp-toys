#include "linalg.h"
#include "draw.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define SIZE 256

double frand(float min, float max)
{
	double core = (double) rand() / (double) RAND_MAX;
	double range = max - min;
	return min + core * range;
}

int main()
{
	time_t t = time(NULL);
	srand((unsigned int) t);

	BMP_t bmp = BMP_create(SIZE, SIZE, 3);
	colour_t red = { 255, 0, 0, 255 };
	colour_t green = { 0, 255, 0, 255 };
	colour_t blue = { 0, 0, 255, 255 };

	mat_t a = mat_row(3, frand(0, SIZE-1), frand(0, SIZE-1), 0);
	mat_t b = mat_row(3, frand(0, SIZE-1), frand(0, SIZE-1), 0);
	mat_t c = mat_row(3, frand(0, SIZE-1), frand(0, SIZE-1), 0);

	mat_t tri = mat_init(3, 3);
	tri = mat_set_row(tri, 0, a);
	tri = mat_set_row(tri, 1, b);
	tri = mat_set_row(tri, 2, c);

	fill_tri(&bmp, green, tri);
	wire_tri(&bmp, red, tri);
	
	BMP_write(&bmp, "test.bmp");

	return 0;
}
