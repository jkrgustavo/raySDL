#pragma once

#include "util.h"

class camera {
public:
    camera(simd::double1 aspect_ratio) {
        simd::double1 viewport_height = 2.0;
        simd::double1 viewport_width = aspect_ratio * viewport_height;
        simd::double1 focal_length = 1.0;

        origin = simd::make_double3(0, 0, 0);
        horizontal = simd::make_double3(viewport_width, 0, 0);
        vertical = simd::make_double3(0, -viewport_height, 0);
        lower_left_corner = origin - horizontal/2 - vertical/2 - simd::make_double3(0, 0, focal_length);
    }
  
    ray get_ray(double u, double v) const {
        return ray(origin, lower_left_corner + u*horizontal + v*vertical - origin);
    }

    void rotate(vec3& rot) {
        double i = origin[0];
        double j = origin[1];
        double k = origin[2];

        double A = rot.x;
        double B = rot.y;
        double C = rot.z;

        origin[0] = j*sin(A)*sin(B)*cos(C) - k*cos(A)*sin(B)*cos(C) +
                        j*cos(A)*sin(B)*sin(C) + k*sin(A)*sin(C) + i*cos(B)*cos(C);
    
        origin[1] = j*cos(A)*cos(C) + k*sin(A)*cos(C) - j*sin(A)*sin(B)*sin(C) +
                        k*cos(A)*sin(B)*sin(C) - i*cos(B)*sin(C);
    
        origin[2] = k*cos(A)*cos(B) - j*sin(A)*cos(B) + i*sin(B);
    }

public:
    point3 origin;
    point3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;
};
