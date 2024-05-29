#pragma once

#include "util.h"
#include <simd/simd.h>
#include <cmath>
#include <iostream>

using std::sqrt;
using std::fabs;

// class vec3 {
// public:
//     vec3() : e{0,0,0} {}
//     vec3(double e0, double e1, double e2) : e{e0, e1, e2} {}
//
//     double x() const { return e[0]; }
//     double y() const { return e[1]; }
//     double z() const { return e[2]; }
//
//     vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
//     double operator[](int i) const { return e[i]; }
//     double& operator[](int i) { return e[i]; }
//
//     vec3& operator=(const vec3 &v) {
//         e[0] = v.e[0];
//         e[1] = v.e[1];
//         e[2] = v.e[2];
//         return *this;
//     }
//
//     vec3& operator+=(const vec3 &v) {
//         e[0] += v.e[0];
//         e[1] += v.e[1];
//         e[2] += v.e[2];
//         return *this;
//     }
//
//     vec3& operator*=(const double t) {
//         e[0] *= t;
//         e[1] *= t;
//         e[2] *= t;
//         return *this;
//     }
//
//     vec3& operator /=(const double t) {
//         return *this *= 1/t;
//     }
//
//     double length() const {
//         return sqrt(length_squared());
//     }
//
//     double length_squared() const {
//         return e[0]*e[0] + e[1]*e[1] + e[2]*e[2];
//     }
//
//     bool near_zero() {
//         auto s = 1e-8;
//         return (e[0] < s && e[1] < s && e[2] < s);
//     }
//
//     inline static vec3 random() {
//         return vec3(random_double(), random_double(), random_double());
//     }
//
//     inline static vec3 random(double min, double max) {
//         return vec3(random_double(min,max), random_double(min,max), random_double(min,max));
//     }
//
//     inline static vec3 pcg_random(uint seed) {
//         return vec3(pcg_random_double(seed), pcg_random_double(seed), pcg_random_double(seed));
//     }
//
//     inline static vec3 pcg_random(uint seed, double min, double max) {
//         return vec3(pcg_random_double(seed, min, max), pcg_random_double(seed, min, max), pcg_random_double(seed, min, max));
//     }
//
//
//
// public:
//     double e[3];
//
// };
//
// // Type aliases for vec3
using point3 = simd::double3;  // 3D point
using color = simd::double3;   // RGB color
using vec3 = simd::double3;

//
// // vec3 Utility Functions
//
// inline std::ostream& operator<<(std::ostream &out, const vec3 &v) {
//     return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
// }
//
// inline vec3 operator+(const vec3 &u, const vec3 &v) {
//     return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
// }
//
// inline vec3 operator-(const vec3 &u, const vec3 &v) {
//     return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
// }
//
// inline vec3 operator*(const vec3 &u, const vec3 &v) {
//     return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
// }
//
// inline vec3 operator*(double t, const vec3 &v) {
//     return vec3(t*v.e[0], t*v.e[1], t*v.e[2]);
// }
//
// inline vec3 operator*(const vec3 &v, double t) {
//     return t * v;
// }
//
// inline vec3 operator/(vec3 v, double t) {
//     return (1/t) * v;
// }
//
// inline bool operator==(const vec3 &u, const vec3 &v) {
//     return u.e[0] == v.e[0] && u.e[1] == v.e[1] && u.e[2] == v.e[2];
// }
//
// inline double dot(const vec3 &u, const vec3 &v) {
//     return u.e[0] * v.e[0]
//          + u.e[1] * v.e[1] 
//          + u.e[2] * v.e[2];
// }
//
// inline vec3 cross(const vec3 &u, const vec3 &v) {
//     return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
//                 u.e[2] * v.e[0] - u.e[0] - v.e[2],
//                 u.e[0] * v.e[1] - u.e[1] - v.e[0]);
// }
//
// inline vec3 unit_vector(vec3 v) {
//     return v / v.length();
// }
//
// inline vec3 reflect(const vec3 &v, const vec3 &n) {
//     return v - 2*dot(v, n)*n; 
// }
//
inline static simd::double3 random3() {
    return simd::make_double3(random_double(), random_double(), random_double());
}

inline static simd::double3 pcg_random3(simd::uint1 &seed) {
    return simd::make_double3(pcg_random_double(seed), pcg_random_double(seed), pcg_random_double(seed));
}

inline static simd::double3 random3(simd::double1 min, simd::double1 max) {
    return simd::make_double3(random_double(min, max), random_double(min, max), random_double(min, max));
}

inline static simd::double3 pcg_random3(simd::uint1 &seed, simd::double1 min, simd::double1 max) {
    return simd::make_double3(pcg_random_double(seed, min, max), pcg_random_double(seed, min, max), pcg_random_double(seed, min, max));
}

vec3 random_in_unit_sphere() {
    while (true) {
        auto p = random3(-1,1);
        if (simd::length_squared(p) >= 1) continue;
        return p;
    }
}

vec3 pcg_in_unit_sphere(uint &seed) {

    for (int safetey_limit = 0; safetey_limit < 100; safetey_limit++) {
        double x = pcg_random_double(seed) * 2 - 1;
        double y = pcg_random_double(seed) * 2 - 1;
        double z = pcg_random_double(seed) * 2 - 1;
        vec3 p = simd::make_double3(x, y, z);

        if (simd::length_squared(p) < 1) {
            return p;
        }
    }

    return vec3();
}

vec3 random_unit_vector() {
    return simd::normalize(random_in_unit_sphere());
}

vec3 pcg_unit_vector(uint &seed) {
    return simd::normalize(pcg_in_unit_sphere(seed));
}

bool near_zero(simd::double3 &v) {
    auto s = 1e-8;
    return (v.x < s && v.y < s && v.z < s);
}
