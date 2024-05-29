#pragma once

#include "hittable.h"
#include "material.h"
#include "sphere.h"

#include <vector>

using std::shared_ptr;
using std::make_shared;



class hittable_list : public hittable {
public:
    hittable_list() {}
    hittable_list(shared_ptr<hittable> object) { add(object); }

    void clear() { objects.clear(); }
    void add(shared_ptr<hittable> object) { objects.emplace_back(object); }

    virtual bool hit(
        const ray& r, simd::double1 t_min, simd::double1 t_max, hit_record& rec) const override;

  
public:
    std::vector<shared_ptr<hittable>> objects;
};

struct Scene {
    std::vector<shared_ptr<material>> materials;
    hittable_list world;
    shared_ptr<hittable> controlled;

    void toggle_controlled() {
        static int i = 1;

        controlled = world.objects[(i++) % world.objects.size()];
    }

    void init_scene1() {
        materials.emplace_back(make_shared<lambertian>(simd::make_double3(0.8, 0.8, 0.0)));
        materials.emplace_back(make_shared<lambertian>(simd::make_double3(0.1, 0.2, 0.5)));

        auto sphere_ground = make_shared<sphere>(simd::make_double3( 0.0, -100.5, -1.0), 100.0, materials[0]);
        auto sphere1 = make_shared<sphere>(simd::make_double3( 0.0,    0.0, -1.2),   0.5, materials[1]);
        world.add(sphere_ground);
        world.add(sphere1);

        controlled = sphere1;
    }
};

bool hittable_list::hit(const ray& r, simd::double1 t_min, simd::double1 t_max, hit_record& rec) const {
    hit_record temp_rec;
    bool hit_anything = false;
    auto closest_so_far = t_max;

    for (const auto& object : objects) {
        if (object->hit(r, t_min, closest_so_far, temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }
    return hit_anything;
}
