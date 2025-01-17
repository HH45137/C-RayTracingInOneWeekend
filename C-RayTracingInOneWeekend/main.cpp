#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <glm\glm.hpp>

#define IMAGE_WIDTH 256
#define IMAGE_HEIGHT 256
#define IMAGE_PIXEL_NUM IMAGE_WIDTH * IMAGE_HEIGHT
#define IMAGE_COLOR_DEPTH 255

/*
* --------------- Typedef ---------------
*/

typedef glm::ivec3 color_t;

/*
* --------------- Function ---------------
*/

int save2ppm(const char* filename, color_t* fb, int width, int height, int color_depth)
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

/*
* --------------- Global Variable ---------------
*/

color_t* fb_array = NULL;	// Framebuffer

/*
* --------------- Main Function ---------------
*/

int main() {

	/*
	* --------------- Init ---------------
	*/

	// Alloc framebuffer array memory
	fb_array = (color_t*)malloc(IMAGE_PIXEL_NUM * sizeof(color_t));

	/*
	* --------------- Start ---------------
	*/

	// Main loop
	for (size_t j = 0; j < IMAGE_HEIGHT; j++)
	{
		for (size_t i = 0; i < IMAGE_WIDTH; i++)
		{
			// Calculate RGB values
			double r = (double)i / (IMAGE_WIDTH - 1);
			double g = (double)j / (IMAGE_HEIGHT - 1);
			double b = 0.0;

			// Clamp to 0~255
			double tem_color_depth = (double)IMAGE_COLOR_DEPTH + 0.999;
			int ir = (int)(tem_color_depth * r);
			int ig = (int)(tem_color_depth * g);
			int ib = (int)(tem_color_depth * b);

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
