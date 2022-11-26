triangle: bmp triangle.c
	gcc-12 bmp.c triangle.c -o triangle.out

dither: bmp dither.c
	gcc-12 bmp.c dither.c -o dither.out

kuwahara: bmp kuwahara.c
	gcc-12 bmp.c kuwahara.c -o kuwahara.out

mandel: bmp mandelbrot.c
	gcc-12 bmp.c mandelbrot.c -o mandelbrot.out

bmp: bmp.c
	gcc-12 -c bmp.c -o bmp.o
