#pragma once

#include <cmath>
#include <limits>
#include <memory>
#include <cstdlib>
#include <iostream>

// Usings
using std::shared_ptr;
using std::make_shared;
using std::sqrt;

// Constants
const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility Functions
inline double degrees_to_radians(double degrees) {
    return degrees * pi/180.0;
}

inline double random_double() {
    return rand() / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max) {
    return min + (max-min)*random_double();
}

inline double pcg_random_double(uint& seed) {
    seed = seed * 747796405 + 2891336453;
	uint result = ((seed >> ((seed >> 28) + 4)) ^ seed) * 277803737;
	result = (result >> 22) ^ result;
    return result / 4294967295.0;
}

inline double pcg_random_double(uint seed, double min, double max) {
    return min + (max-min)*pcg_random_double(seed);
}

inline double clamp(double x, double min, double max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

#define NOW() (std::chrono::high_resolution_clock::now())
#define GET_TIME(_c, _l) (std::chrono::duration_cast<duration<double>>(_c - _l).count())

// Common Headers
#include "ray.h"
#include "vec3.h"

