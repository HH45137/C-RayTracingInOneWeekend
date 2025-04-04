#pragma once

#include "hit_table.h"
#include "common.h"

class camera
{
public:
	// Image info
	double aspect_ratio = 16.0 / 9.0;
	int32_t image_width = 512;
	int32_t image_height;
	int32_t image_pixel_count;
	int32_t image_bit_depth;

	// Camera info
	double viewport_height;
	double viewport_width;
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

	// Ray info
	int32_t max_depth = 10;

	void render(hit_table& world);

	camera();
	~camera();

private:
	void initialize();

	void clean();

	dcolor_t ray_color(ray& r, int32_t depth, hit_table& world);

	int save2ppm(const char* filename, icolor_t* fb, int width, int height, int color_depth);

	ray gen_ray(int32_t i, int32_t j);
};

camera::camera()
{
}

camera::~camera()
{
	this->clean();
}

void camera::render(hit_table& world)
{
	this->initialize();

	printf("Rendering image....\n");
	// Main loop
	for (size_t j = 0; j < image_height; j++)
	{
		for (size_t i = 0; i < image_width; i++)
		{
			dcolor_t pixel{ 0 };

			// Calculate RGB values
			for (size_t sample = 0; sample < samples_per_pixel; sample++)
			{
				ray r = gen_ray(i, j);
				pixel += ray_color(r, max_depth, world);
			}
			pixel *= pixel_samples_scale;

			// Clamp to 0~255
			double tem_color_depth = (double)image_bit_depth + 0.999;
			int ir = (int)(tem_color_depth * linear_to_gamma(pixel.r));
			int ig = (int)(tem_color_depth * linear_to_gamma(pixel.g));
			int ib = (int)(tem_color_depth * linear_to_gamma(pixel.b));

			// Get framebuffer 1D array index
			size_t fb_idx = j * image_width + i;

			// Set framebuffer pixel RGB values
			fb_array[fb_idx].r = ir;
			fb_array[fb_idx].g = ig;
			fb_array[fb_idx].b = ib;

#ifdef _DEBUG
			//printf("fb_idx = %zd \t ir = %d \t ig = %d \t ib = %d \n", fb_idx, ir, ig, ib);
#endif
		}
	}
	printf("Render image done.\n");

	printf("Saveing image....\n");
	// Save framebuffer to PPM image
	save2ppm("./output.ppm", fb_array, image_width, image_height, image_bit_depth);
	printf("Save image done.\n");
}

void camera::initialize()
{
	image_height = image_width / aspect_ratio;
	image_pixel_count = image_width * image_height;
	image_bit_depth = 255;

	viewport_height = 2.0;
	viewport_width = viewport_height * aspect_ratio;
	focal_length = 1.0;
	camera_center = dpoint_t(0, 0, 0);

	viewport_u = dvec3_t(viewport_width, 0, 0);
	viewport_v = dvec3_t(0, -viewport_height, 0);

	pixel_delta_u = viewport_u / (double)image_width;
	pixel_delta_v = viewport_v / (double)image_height;

	viewport_upper_left = camera_center - dvec3_t(0, 0, focal_length) - viewport_u / 2.0 - viewport_v / 2.0;
	pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

	pixel_samples_scale = 1.0 / samples_per_pixel;

	// Alloc framebuffer array memory
	fb_array = (icolor_t*)malloc(image_pixel_count * sizeof(icolor_t));
}

void camera::clean()
{
	// Free framebuffer array memory
	free(fb_array);
}

dcolor_t camera::ray_color(ray& r, int32_t depth, hit_table& world)
{
	dcolor_t pixel{ 0 };

	if (depth <= 0)
	{
		return pixel;
	}

	// Claculate sphere pixel
	dcolor_t sphere_color{};
	hit_record rec;
	if (world.hit(r, interval(0.001, INFINITY_DOUBLE), rec))
	{
		//	dvec3_t dir = rec.normal + random_unit_vector();
		//	ray new_ray = ray(rec.p, dir);
		//	sphere_color = 0.2 * ray_color(new_ray, depth - 1, world);
		//	return sphere_color;

		ray scattered{};
		dcolor_t attenuation;
		if (rec.mat->scatter(r, rec, attenuation, scattered)) {
			attenuation* ray_color(scattered, depth - 1, world);
		}
		return dcolor_t{ 0.0 };
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
