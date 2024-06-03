#pragma once

#include "hittable.h"
#include "util.h"


class material {
public:
    virtual ~material() = default;

    // scatter based on the cstdlib rand()
    virtual bool scatter(
        const ray &r_in, const hit_record &rec, color &attenuation, ray &scattered) const {
        return false;
    }

    // scatter based on the pcg hashing function
    virtual bool scatter(
        const ray &r_in, const hit_record &rec, color &attenuation, ray &scattered, uint seed)
    const {
        return false;
    }

    virtual color get_color() const { return simd::make_float3(0, 0, 0); }
    virtual bool set_color(const color &in) { return false; }
    virtual const char *type_name() const { return "material"; }
};

class lambertian : public material {
public:

    lambertian(const color &albedo) : albedo(albedo) {}

    bool scatter(
        const ray &r_in, const hit_record &rec, color &attenuation, ray &scattered) 
    const override {

        auto scatter_direction = rec.normal + random_unit_vector();

        if (near_zero(scatter_direction))
            scatter_direction = rec.normal;

        scattered = ray(rec.p, scatter_direction);
        attenuation = albedo;
        return true;
    }

    bool scatter(
        const ray &r_in, const hit_record &rec, color &attenuation, ray &scattered, uint seed) 
    const override {

        auto scatter_direction = rec.normal + pcg_unit_vector(seed);

        if (near_zero(scatter_direction))
            scatter_direction = rec.normal;

        scattered = ray(rec.p, scatter_direction);
        attenuation = albedo;
        return true;
    }

    color get_color() const override { return albedo; }
    bool set_color(const color &in) override { albedo = in; return true; }
    const char *type_name() const override { return "lambertian"; }

public:
    color albedo;

};

class metal : public material {
public:

    metal(const color &albedo) : albedo(albedo) {}

    bool scatter(
        const ray &r_in, const hit_record &rec, color &attenuation, ray &scattered) 
    const override {
        auto reflected = simd::reflect(r_in.direction(), rec.normal);
        scattered = ray(rec.p, reflected);
        attenuation = albedo;
        return true;
    }

    bool scatter(
        const ray &r_in, const hit_record &rec, color &attenuation, ray &scattered, uint seed) 
    const override {
        auto reflected = simd::reflect(r_in.direction(), rec.normal);
        scattered = ray(rec.p, reflected);
        attenuation = albedo;
        return true;
    }

    color get_color() const override { return albedo; }
    bool set_color(const color &in) override { albedo = in; return true; }
    const char *type_name() const override { return "metal"; }

public:
    color albedo;
};
