#pragma once

#include "util.h"
#include <arm_neon.h>

class camera {
public:
    camera(simd::float1 aspect_ratio);
  
    ray get_ray(float u, float v) const;

public:
    point3 origin;
    point3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;
};

camera::camera(simd::float1 aspect_ratio) {
        simd::float1 viewport_height = 2.0;
        simd::float1 viewport_width = aspect_ratio * viewport_height;
        simd::float1 focal_length = 1.0;

        origin = simd::make_float3(0, 0, 0);
        horizontal = simd::make_float3(viewport_width, 0, 0);
        vertical = simd::make_float3(0, -viewport_height, 0);
        lower_left_corner = origin - horizontal/2 - vertical/2 - simd::make_float3(0, 0, focal_length);
}

ray camera::get_ray(float u, float v) const {
    return ray(origin, lower_left_corner + u*horizontal + v*vertical - origin);
}
