#pragma once

#include "common.h"

class ray
{
private:

public:
	dpoint_t origin;
	dvec3_t direction;

	ray();
	
	ray(const dpoint_t& ori, const dvec3_t dir);

	dpoint_t at(double t) const;

};
