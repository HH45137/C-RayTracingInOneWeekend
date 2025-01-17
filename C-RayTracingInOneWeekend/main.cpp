#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <glm\glm.hpp>
#include "typedef.h"
#include "ray.h"

#define IMAGE_WIDTH 256
#define IMAGE_HEIGHT 256
#define IMAGE_PIXEL_NUM IMAGE_WIDTH * IMAGE_HEIGHT
#define IMAGE_COLOR_DEPTH 255

/*
* --------------- Function ---------------
*/

int save2ppm(const char* filename, icolor_t* fb, int width, int height, int color_depth)
{
	FILE* fp = NULL;

	fp = fopen(filename, "w");
	if (fp == NULL)
	{
		perror("Error opening or create ppm file!\n");
		return 1;
	}

	fprintf(fp, "P3\n");
	fprintf(fp, "%d %d\n", width, height);
	fprintf(fp, "%d\n", color_depth);

	for (size_t j = 0; j < height; j++)
	{
		for (size_t i = 0; i < width; i++)
		{
			// Get framebuffer 1D array index
			size_t fb_idx = j * width + i;

			// Write RGB to fp
			fprintf(fp, "%d %d %d\n", fb[fb_idx].r, fb[fb_idx].g, fb[fb_idx].b);
		}
	}

	fclose(fp);

	return 0;
}

dcolor_t render(size_t x, size_t y) 
{
	dcolor_t pixel_color{};

	pixel_color = dcolor_t{
		double(x) / (IMAGE_WIDTH - 1),
		double(y) / (IMAGE_HEIGHT - 1),
		double(0.0),
	};

	return pixel_color;
}

/*
* --------------- Global Variable ---------------
*/

icolor_t* fb_array = NULL;	// Framebuffer

/*
* --------------- Main Function ---------------
*/

int main() {

	/*
	* --------------- Init ---------------
	*/

	// Alloc framebuffer array memory
	fb_array = (icolor_t*)malloc(IMAGE_PIXEL_NUM * sizeof(icolor_t));

	/*
	* --------------- Start ---------------
	*/

	// Main loop
	for (size_t j = 0; j < IMAGE_HEIGHT; j++)
	{
		for (size_t i = 0; i < IMAGE_WIDTH; i++)
		{
			// Calculate RGB values
			dcolor_t pixel = render(i, j);

			// Clamp to 0~255
			double tem_color_depth = (double)IMAGE_COLOR_DEPTH + 0.999;
			int ir = (int)(tem_color_depth * pixel.r);
			int ig = (int)(tem_color_depth * pixel.g);
			int ib = (int)(tem_color_depth * pixel.b);

			// Get framebuffer 1D array index
			size_t fb_idx = j * IMAGE_WIDTH + i;

			// Set framebuffer pixel RGB values
			fb_array[fb_idx].r = ir;
			fb_array[fb_idx].g = ig;
			fb_array[fb_idx].b = ib;

			printf("fb_idx = %zd \t ir = %d \t ig = %d \t ib = %d \n", fb_idx, ir, ig, ib);
		}
	}

	// Save framebuffer to PPM image
	save2ppm("./output.ppm", fb_array, IMAGE_WIDTH, IMAGE_HEIGHT, IMAGE_COLOR_DEPTH);

	/*
	* --------------- Clean ---------------
	*/

	// Free framebuffer array memory
	free(fb_array);


	system("PAUSE");
	return 0;
}
