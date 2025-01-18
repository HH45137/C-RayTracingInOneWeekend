#pragma once

#include "hit_table.h"

class sphere : public hit_table
{
public:
	sphere(const dpoint_t& center, double radius, const ray& r);

	bool hit(ray& r, double ray_t_min, double ray_t_max, hit_record& rec) const override;

private:
	dpoint_t center;
	double radius;
};

sphere::sphere(const dpoint_t& center, double radius, const ray& r) : center(center), radius(MAX(0.0, radius))
{
}

bool sphere::hit(ray& r, double ray_t_min, double ray_t_max, hit_record& rec) const
{
	dvec3_t oc = center - r.origin;
	auto a = LENGTH_SQUARED(r.direction);
	auto h = DOT(r.direction, oc);
	auto c = LENGTH_SQUARED(oc) - radius * radius;

	auto disriminant = h * h - a * c;
	if (disriminant < 0.0)
	{
		return false;
	}

	auto sqrtd = SQRT(disriminant);

	// Find the nearest root that lies in the acceptable range.
	auto root = (h - sqrtd) / a;
	if (root <= ray_t_min || ray_t_max <= root)
	{
		root = (h + sqrtd) / a;
		if (root <= ray_t_min || ray_t_max <= root)
		{
			return false;
		}
	}

	rec.t = root;
	rec.p = r.at(rec.t);
	dvec3_t outward_normal = (rec.p - center) / radius;
	rec.set_face_normal(r, outward_normal);

	return true;
}