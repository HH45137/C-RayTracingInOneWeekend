#include "material.h"

inline bool lambertian::scatter(const ray& r_in, const hit_record& rec, dcolor_t& attenuation, ray& scattered) const {
	auto scatter_dir = rec.normal + random_unit_vector();

	if (near_zero_vector(scatter_dir)) {
		scatter_dir = rec.normal;
	}

	scattered = ray(rec.p, scatter_dir);
	attenuation = albedo;
	return true;
}

inline bool metal::scatter(const ray& r_in, const hit_record& rec, dcolor_t& attenuation, ray& scattered) const {
	auto reflected = reflect(r_in.direction, rec.normal);
	scattered = ray(rec.p, reflected);
	attenuation = albedo;
	return true;
}

inline bool material::scatter(const ray& r_in, const hit_record& rec, dcolor_t& attenuation, ray& scattered) const {
	return false;
}
