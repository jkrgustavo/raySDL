#pragma once

#include "util.h"
#include <simd/simd.h>
#include <cmath>

using std::sqrt;
using std::fabs;

// Type aliases
using point3 = simd::float3;  // 3D point
using color = simd::float3;   // RGB color
using vec3 = simd::float3;    // 3D vector

inline static simd::float3 random3() {
    return simd::make_float3(random_float(), random_float(), random_float());
}

inline static simd::float3 pcg_random3(simd::uint1 &seed) {
    return simd::make_float3(pcg_random_float(seed), pcg_random_float(seed), pcg_random_float(seed));
}

inline static simd::float3 random3(simd::double1 min, simd::double1 max) {
    return simd::make_float3(random_float(min, max), random_float(min, max), random_float(min, max));
}

inline static simd::float3 pcg_random3(simd::uint1 &seed, simd::double1 min, simd::double1 max) {
    return simd::make_float3(pcg_random_float(seed, min, max), pcg_random_float(seed, min, max), pcg_random_float(seed, min, max));
}

vec3 random_in_unit_sphere() {
    while (true) {
        auto p = random3(-1,1);
        if (simd::length_squared(p) >= 1) continue;
        return p;
    }
}

vec3 pcg_in_unit_sphere(simd::uint1 &seed) {

    for (int safetey_limit = 0; safetey_limit < 100; safetey_limit++) {
        float x = pcg_random_float(seed) * 2 - 1;
        float y = pcg_random_float(seed) * 2 - 1;
        float z = pcg_random_float(seed) * 2 - 1;
        vec3 p = simd::make_float3(x, y, z);

        if (simd::length_squared(p) < 1) {
            return p;
        }
    }

    return vec3();
}

vec3 random_unit_vector() {
    return simd::normalize(random_in_unit_sphere());
}

vec3 pcg_unit_vector(simd::uint1 &seed) {
    return simd::normalize(pcg_in_unit_sphere(seed));
}

bool near_zero(simd::float3 &v) {
    auto s = 1e-8;
    return (v.x < s && v.y < s && v.z < s);
}
