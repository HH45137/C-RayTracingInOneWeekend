#pragma once

#include "common.h"

class interval
{
public:
	double min, max;

	static const interval empty, universe;

	interval();

	interval(double min, double max);

	double size();

	bool contains(double x);

	bool surrounds(double x);

private:

};

const interval interval::empty = interval(+INFINITY_DOUBLE, -INFINITY_DOUBLE);
const interval interval::universe = interval(-INFINITY_DOUBLE, +INFINITY_DOUBLE);

interval::interval() : min(+INFINITY_DOUBLE), max(-INFINITY_DOUBLE)
{
}

interval::interval(double min, double max)
	: min(min), max(max)
{
}

double interval::size() { return max - min; }

bool interval::contains(double x) { return min <= x && x <= max; }

bool interval::surrounds(double x) { return min < x&& x < max; }
