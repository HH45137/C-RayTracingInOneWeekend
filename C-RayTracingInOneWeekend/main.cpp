#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "ray.h"
#include "hit_table_list.h"
#include "sphere.h"


/*
* --------------- Global Variable ---------------
*/

// Image info
const auto ASPECT_RATIO = 16.0 / 9.0;
const int32_t IMAGE_WIDTH = 256;
const int32_t IMAGE_HEIGHT = IMAGE_WIDTH / ASPECT_RATIO;
const int32_t IMAGE_PIXEL_NUM = IMAGE_WIDTH * IMAGE_HEIGHT;
const int32_t IMAGE_COLOR_DEPTH = 255;

// Camera info
const auto VIEWPORT_HEIGHT = 2.0;
const auto VIEWPORT_WIDTH = VIEWPORT_HEIGHT * ASPECT_RATIO;
auto focal_length = 1.0;
auto camera_center = dpoint_t(0, 0, 0);
// Calculate the vectors across the horizontal and down the vertical viewport edges.
auto viewport_u = dvec3_t(VIEWPORT_WIDTH, 0, 0);
auto viewport_v = dvec3_t(0, -VIEWPORT_HEIGHT, 0);
// Calculate the horizontal and vertical delta vectors from pixel to pixel.
auto pixel_delta_u = viewport_u / (double)IMAGE_WIDTH;
auto pixel_delta_v = viewport_v / (double)IMAGE_HEIGHT;
// Calculate the location of the upper left pixel.
auto viewport_upper_left = camera_center - dvec3_t(0, 0, focal_length) - viewport_u / 2.0 - viewport_v / 2.0;
auto pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);


// Framebuffer
icolor_t* fb_array = NULL;

// World (All objects)
hit_table_list world;

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

int init() 
{
	// Alloc framebuffer array memory
	fb_array = (icolor_t*)malloc(IMAGE_PIXEL_NUM * sizeof(icolor_t));

	// Add objects into world
	{
		world.add(make_shared<sphere>(dpoint_t{ 0,0,-1 }, .5));
		world.add(make_shared<sphere>(dpoint_t{ 0,-100.5,-1 }, 100.0));
	}

	return 0;
}

dcolor_t ray_color(ray& r, const hit_table& world)
{
	dcolor_t pixel{};

	// Claculate sphere pixel
	dcolor_t sphere_color{};
	hit_record rec;
	if (world.hit(r, interval(0.0, INFINITY_DOUBLE), rec))
	{
		sphere_color = 0.5 * (rec.normal + dcolor_t{ 1 });
		return sphere_color;
	}

	// Calculate background pixel
	dcolor_t background_color{};
	dvec3_t unit_dir = NORMALIZE(r.direction);
	auto a = 0.5 * (unit_dir.y + 1.0);
	background_color = (1.0 - a) * dcolor_t { 1.0 } + a * dcolor_t{ .5,.7,1.0 };

	// blend all
	pixel = background_color;

	return pixel;
}

dcolor_t render(size_t x, size_t y) 
{
	dcolor_t pixel_color{};

	auto pixel_center = pixel00_loc + ((double)x * pixel_delta_u) + ((double)y * pixel_delta_v);
	auto ray_dir = pixel_center - camera_center;

	ray r(camera_center, pixel_center);
	pixel_color = ray_color(r, world);

	return pixel_color;
}

/*
* --------------- Main Function ---------------
*/

int main() {

	/*
	* --------------- Init ---------------
	*/

	init();

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
