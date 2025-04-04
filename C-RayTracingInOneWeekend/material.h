#pragma once

#include "hit_table.h"

class material
{
public:
	virtual ~material() = default;

	virtual bool scatter(
		const ray& r_in,
		const hit_record& rec,
		dcolor_t& attenuation,
		ray& scattered
	) const;
};

class lambertian : public material
{
public:
	lambertian(const dcolor_t& albedo) :albedo(albedo) {}

	bool scatter(
		const ray& r_in,
		const hit_record& rec,
		dcolor_t& attenuation,
		ray& scattered
	) const override;

private:
	dcolor_t albedo;
};

class metal : public material
{
public:
	metal(const dcolor_t& albedo) :albedo(albedo) {}

	bool scatter(
		const ray& r_in,
		const hit_record& rec,
		dcolor_t& attenuation,
		ray& scattered
	) const override;

private:
	dcolor_t albedo;
};

