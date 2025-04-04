#pragma once

#include "ray.h"
#include "interval.h"
#include "material.h"

class material;

class hit_record
{
public:
	dpoint_t p;
	dvec3_t normal;
	double t;
	bool front_face;
	shared_ptr<material> mat;

	void set_face_normal(ray& r, const dvec3_t outward_normal);

private:

};

// The outward_normal must be normalized
void hit_record::set_face_normal(ray& r, const dvec3_t outward_normal)
{
	this->front_face = DOT(r.direction, outward_normal) < 0.0;
	this->normal = front_face ? outward_normal : -outward_normal;
}

class hit_table
{
public:
	virtual ~hit_table() = default;

	virtual bool hit(ray& r, interval ray_t, hit_record& rec) const = 0;

private:

};
