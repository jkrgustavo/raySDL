#pragma once

#include "util.h"

class material;

struct hit_record {
    point3 p;
    vec3 normal;
    shared_ptr<material> mat;
    double t;
    bool front_face;

    inline void set_face_normal(const ray& r, const vec3& outward_normal) {
        front_face = simd::dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class hittable {
public:
    virtual bool hit(const ray& r, simd::double1 t_min, simd::double1 t_max, hit_record& rec) const = 0;
    virtual ~hittable() {}
};
