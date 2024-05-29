#pragma once

#include "render.h"
#include "util.h"
#include "vec3.h"

color set_color(color &pixel_color, simd::int1 samples_per_pixel) {
    auto pr = pixel_color.x;
    auto pg = pixel_color.y;
    auto pb = pixel_color.z;

    // Divide the color by the number of samples and gamma-correct for gamma=2.0
    auto scale = 1.0 / samples_per_pixel;
    pr = sqrt(scale * pr);
    pg = sqrt(scale * pg);
    pb = sqrt(scale * pb);

    auto r = static_cast<int>(256 * simd::clamp(pr, 0.0, 0.999));
    auto g = static_cast<int>(256 * simd::clamp(pg, 0.0, 0.999));
    auto b = static_cast<int>(256 * simd::clamp(pb, 0.0, 0.999));

    return simd::make_double3(r, g, b);
}

static inline uint pack_color(color &pixel_color, int samples_per_pixel) {
    static simd::double3 lower = { 0.0, 0.0, 0.0 };
    static simd::double3 upper = { 0.999, 0.999, 0.999 };

    // Divide the color by the number of samples and gamma-correct for gamma=2.0
    simd::double1 scale = simd::recip(static_cast<double>(samples_per_pixel));

    simd::double3 gamma_corrected = simd::sqrt(pixel_color * scale);
    simd::uint3 clamped = simd::convert_sat<uint>(256 * simd::clamp(gamma_corrected, lower, upper));

    return (0xFF << 24) | (clamped.z << 16) | (clamped.y << 8) | clamped.x;
}

void write_color(Renderer &renderer, color pixel_color, int samples_per_pixel, int x, int y) {
    color pixel = set_color(pixel_color, samples_per_pixel);
    renderer.set_pixel(x, y, pixel.x, pixel.y, pixel.z);
}

void accumulate(Renderer& renderer, color old_buffer[], color new_buffer[], int samples_per_pixel) {
    double weight = 1.0 / (renderer.frame_count() + 1.0);
    for (int j = renderer.get_texture_height()-1; j >= 0; j--) {
        for (int i = 0; i < renderer.get_texture_width(); i++) {
            int index = (j * renderer.get_texture_width()) + i;
            new_buffer[index] = set_color(new_buffer[index], samples_per_pixel);
            new_buffer[index] = old_buffer[index] * (1 - weight) + new_buffer[index] * weight;
            renderer.set_pixel(i, j, new_buffer[index].x, new_buffer[index].y, new_buffer[index].z);
        }
    }  
}
