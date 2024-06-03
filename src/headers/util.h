#pragma once

#include <simd/simd.h>
#include <memory>
#include <sys/types.h>




// Usings
using std::shared_ptr;
using std::make_shared;
using std::sqrt;

// Constants
const simd::float1 infinity = std::numeric_limits<simd::float1>::infinity();
const simd::float1 pi = 3.1415926535897932385;

// Utility Functions
inline simd::float1 degrees_to_radians(simd::float1 degrees) {
    return degrees * pi/180.0;
}

inline simd::float1 random_float() {
    return rand() / (RAND_MAX + 1.0);
}

inline simd::float1 random_float(simd::float1 min, simd::float1 max) {
    return min + (max-min)*random_float();
}

inline simd::float1 pcg_random_float(simd::uint1& seed) {
    seed = seed * 747796405 + 2891336453;
    simd::uint1 result = ((seed >> ((seed >> 28) + 4)) ^ seed) * 277803737;
	result = (result >> 22) ^ result;
    return result / 4294967295.0;
}

inline simd::float1 pcg_random_float(simd::uint1 seed, simd::float1 min, simd::float1 max) {
    return min + (max-min)*pcg_random_float(seed);
}

#define NOW() (std::chrono::high_resolution_clock::now())
#define GET_TIME(_c, _l) (std::chrono::duration_cast<std::chrono::duration<float>>(_c - _l).count())

// Common Headers
#include "ray.h"
#include "vec3.h"
