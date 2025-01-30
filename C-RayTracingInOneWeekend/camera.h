#pragma once

#include "hit_table.h"
#include "common.h"

class camera
{
public:
	// Image info
	double ASPECT_RATIO;
	int32_t IMAGE_WIDTH;
	int32_t IMAGE_HEIGHT;
	int32_t IMAGE_PIXEL_NUM;
	int32_t IMAGE_COLOR_DEPTH;

	// Camera info
	double VIEWPORT_HEIGHT;
	double VIEWPORT_WIDTH;
	double focal_length;
	dpoint_t camera_center;
	// Calculate the vectors across the horizontal and down the vertical viewport edges.
	dvec3_t viewport_u;
	dvec3_t viewport_v;
	// Calculate the horizontal and vertical delta vectors from pixel to pixel.
	dvec3_t pixel_delta_u;
	dvec3_t pixel_delta_v;
	// Calculate the location of the upper left pixel.
	dvec3_t viewport_upper_left;
	dvec3_t pixel00_loc;

	// Framebuffer
	icolor_t* fb_array = NULL;

	// MSAA info
	int32_t samples_per_pixel;
	double pixel_samples_scale;

	void render(hit_table& world);

	camera(int32_t image_w, double image_aspect_ratio);
	~camera();

private:
	void initialize(int32_t image_w, double image_aspect_ratio);

	void clean();

	dcolor_t ray_color(ray& r, hit_table& world);

	int save2ppm(const char* filename, icolor_t* fb, int width, int height, int color_depth);

	ray gen_ray(int32_t i, int32_t j);
};

void camera::render(hit_table& world)
{
	printf("Rendering image....\n");
	// Main loop
	for (size_t j = 0; j < IMAGE_HEIGHT; j++)
	{
		for (size_t i = 0; i < IMAGE_WIDTH; i++)
		{
			dcolor_t pixel{ 0 };

			// Calculate RGB values
			for (size_t sample = 0; sample < samples_per_pixel; sample++)
			{
				ray r = gen_ray(i, j);
				pixel += ray_color(r, world);
			}
			pixel *= pixel_samples_scale;

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

#ifdef _DEBUG
			printf("fb_idx = %zd \t ir = %d \t ig = %d \t ib = %d \n", fb_idx, ir, ig, ib);
#endif
		}
	}
	printf("Render image done.\n");

	printf("Saveing image....\n");
	// Save framebuffer to PPM image
	save2ppm("./output.ppm", fb_array, IMAGE_WIDTH, IMAGE_HEIGHT, IMAGE_COLOR_DEPTH);
	printf("Save image done.\n");
}

camera::camera(int32_t image_w, double image_aspect_ratio)
{
	this->initialize(image_w, image_aspect_ratio);
}

camera::~camera()
{
	this->clean();
}

void camera::initialize(int32_t image_w, double image_aspect_ratio)
{
	ASPECT_RATIO = image_aspect_ratio;
	IMAGE_WIDTH = image_w;
	IMAGE_HEIGHT = IMAGE_WIDTH / ASPECT_RATIO;
	IMAGE_PIXEL_NUM = IMAGE_WIDTH * IMAGE_HEIGHT;
	IMAGE_COLOR_DEPTH = 255;

	VIEWPORT_HEIGHT = 2.0;
	VIEWPORT_WIDTH = VIEWPORT_HEIGHT * ASPECT_RATIO;
	focal_length = 1.0;
	camera_center = dpoint_t(0, 0, 0);

	viewport_u = dvec3_t(VIEWPORT_WIDTH, 0, 0);
	viewport_v = dvec3_t(0, -VIEWPORT_HEIGHT, 0);

	pixel_delta_u = viewport_u / (double)IMAGE_WIDTH;
	pixel_delta_v = viewport_v / (double)IMAGE_HEIGHT;

	viewport_upper_left = camera_center - dvec3_t(0, 0, focal_length) - viewport_u / 2.0 - viewport_v / 2.0;
	pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

	samples_per_pixel = 8;
	pixel_samples_scale = 1.0 / samples_per_pixel;

	// Alloc framebuffer array memory
	fb_array = (icolor_t*)malloc(IMAGE_PIXEL_NUM * sizeof(icolor_t));
}

void camera::clean()
{
	// Free framebuffer array memory
	free(fb_array);
}

dcolor_t camera::ray_color(ray& r, hit_table& world)
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
	background_color = (1.0 - a) * dcolor_t{ 1.0 } + a * dcolor_t{ .5,.7,1.0 };

	// blend all
	pixel = background_color;

	return pixel;
}

int camera::save2ppm(const char* filename, icolor_t* fb, int width, int height, int color_depth)
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

ray camera::gen_ray(int32_t i, int32_t j)
{
	auto offset = sample_square();
	auto pixel_sample_point =
		pixel00_loc
		+ (((double)i + offset.x) * pixel_delta_u)
		+ (((double)j + offset.y) * pixel_delta_v);
	
	ray r(camera_center, pixel_sample_point - camera_center);

	return r;
}
