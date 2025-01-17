#include "ray.h"

ray::ray()
{
}

ray::ray(const dpoint_t& ori, const dvec3_t dir) : origin(ori), direction(dir)
{
}

dpoint_t ray::at(double t)
{
	return this->origin + t * this->direction;
}
