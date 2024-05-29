#pragma once

#include <cmath>
#include <limits>
#include <memory>
#include <cstdlib>
#include <iostream>
#include <simd/simd.h>

// Usings
using std::shared_ptr;
using std::make_shared;
using std::sqrt;

// Constants
const simd::double1 infinity = std::numeric_limits<simd::double1>::infinity();
const simd::double1 pi = 3.1415926535897932385;

// Utility Functions
inline simd::double1 degrees_to_radians(simd::double1 degrees) {
    return degrees * pi/180.0;
}

inline simd::double1 random_double() {
    return rand() / (RAND_MAX + 1.0);
}

inline simd::double1 random_double(simd::double1 min, simd::double1 max) {
    return min + (max-min)*random_double();
}

inline simd::double1 pcg_random_double(simd::uint1& seed) {
    seed = seed * 747796405 + 2891336453;
    simd::uint1 result = ((seed >> ((seed >> 28) + 4)) ^ seed) * 277803737;
	result = (result >> 22) ^ result;
    return result / 4294967295.0;
}

inline simd::double1 pcg_random_double(simd::uint1 seed, simd::double1 min, simd::double1 max) {
    return min + (max-min)*pcg_random_double(seed);
}

#define NOW() (std::chrono::high_resolution_clock::now())
#define GET_TIME(_c, _l) (std::chrono::duration_cast<std::chrono::duration<double>>(_c - _l).count())

// Common Headers

#include "ray.h"
#include "vec3.h"



