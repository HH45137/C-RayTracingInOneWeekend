#pragma once

#include "ray.h"

class hit_record
{
public:
	dpoint_t p;
	dvec3_t normal;
	double t;

private:

};

class hit_table
{
public:
	virtual ~hit_table() = default;

	virtual bool hit(ray& r, double ray_t_min, double ray_t_max, hit_record& rec) const = 0;

private:

};