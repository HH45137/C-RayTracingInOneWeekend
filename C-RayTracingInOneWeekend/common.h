#pragma once

#include <glm\glm.hpp>

#include <memory>
#include <vector>
#include <cmath>
#include <limits>


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
