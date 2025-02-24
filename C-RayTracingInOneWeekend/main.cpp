﻿#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "ray.h"
#include "hit_table_list.h"
#include "sphere.h"
#include "camera.h"


int main() {

	// World (All objects)
	hit_table_list world;
	world.add(make_shared<sphere>(dpoint_t{ 0,0,-1 }, .5));
	world.add(make_shared<sphere>(dpoint_t{ 0,-100.5,-1 }, 100.0));

	camera camera01{};
	camera01.image_width = 640;
	camera01.aspect_ratio = 16.0 / 9.0;
	camera01.samples_per_pixel = 8;
	camera01.max_depth = 50;

	camera01.render(world);

	system("PAUSE");
	return 0;
}
