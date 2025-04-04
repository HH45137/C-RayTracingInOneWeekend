#pragma once

#include <glm\glm.hpp>

#include <memory>
#include <vector>
#include <cmath>
#include <limits>
#include <cstdlib>


/*
* --------------- Namespace ---------------
*/

using std::make_shared;
using std::shared_ptr;

/*
* --------------- Typedef ---------------
*/

typedef glm::dvec3 dvec3_t;
typedef glm::ivec3 icolor_t;
typedef dvec3_t dcolor_t;
typedef dvec3_t dpoint_t;

/*
* --------------- Funcdef ---------------
*/

#define NORMALIZE glm::normalize
#define DOT glm::dot
#define SQRT glm::sqrt
#define LENGTH_SQUARED(v) glm::dot(v, v)
#define MAX glm::max

/*
* --------------- Constants ---------------
*/

const double INFINITY_DOUBLE = std::numeric_limits<double>::infinity();
const double PI_DOUBLE = 3.1415926535897932385;

/*
* --------------- Function ---------------
*/

static double random_double()
{
	return std::rand() / (RAND_MAX + 1.0);
}

static double random_double(double min, double max)
{
	return min + (max - min) * random_double();
}

static dvec3_t sample_square() 
{
	return dvec3_t(random_double() - 0.5, random_double() - 0.5, 0);
}

static dvec3_t random_vec3()
{
	return dvec3_t(random_double(), random_double(), random_double());
}

static dvec3_t random_vec3(double min, double max)
{
	return dvec3_t(random_double(min, max), random_double(min, max), random_double(min, max));
}

static dvec3_t random_unit_vector() 
{
	while (true) {
		auto p = random_vec3(-1, 1);
		auto lensq = LENGTH_SQUARED(p);
		if (1e-160 < lensq && lensq <= 1)
		{
			return p / SQRT(lensq);
		}
	}
}

static dvec3_t random_on_halfsphere(dvec3_t& normal)
{
	dvec3_t on_unit_sphere = random_unit_vector();
	if (DOT(on_unit_sphere, normal) > 0.0)
	{
		return on_unit_sphere;
	}
	else
	{
		return -on_unit_sphere;
	}
}

static double linear_to_gamma(double linear_component)
{
	if (linear_component > 0)
	{
		return SQRT(linear_component);
	}

	return 0;
}

static bool near_zero_vector(dvec3_t vec) {
	auto s = 1e-8;
	return (std::fabs(vec[0] < s) && std::fabs(vec[1] < s) && std::fabs(vec[2] < s));
}

// 得到一根光线的反射方向的向量
static dvec3_t reflect(const dvec3_t& v, const dvec3_t& n) {
	return v - 2 * DOT(v, n) * n;
}
