#pragma once

#include "typedef.h"

class ray
{
private:
	dpoint_t origin;
	dvec3_t direction;

public:
	ray();
	
	ray(const dpoint_t& ori, const dvec3_t dir);

	dpoint_t at(double t);

};
