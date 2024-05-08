#pragma once

#include "render.h"
#include "util.h"
#include "vec3.h"

color set_color(color &pixel_color, int samples_per_pixel) {
  auto pr = pixel_color.x();
  auto pg = pixel_color.y();
  auto pb = pixel_color.z();

  // Divide the color by the number of samples and gamma-correct for gamma=2.0
  auto scale = 1.0 / samples_per_pixel;
  pr = sqrt(scale * pr);
  pg = sqrt(scale * pg);
  pb = sqrt(scale * pb);

  auto r = static_cast<int>(256 * clamp(pr, 0.0, 0.999));
  auto g = static_cast<int>(256 * clamp(pg, 0.0, 0.999));
  auto b = static_cast<int>(256 * clamp(pb, 0.0, 0.999));

  return color(r, g, b);
}

void write_color(Renderer &renderer, color pixel_color, int samples_per_pixel, int x, int y) {
  color pixel = set_color(pixel_color, samples_per_pixel);
  renderer.set_pixel(x, y, pixel.x(), pixel.y(), pixel.z());
}

void accumulate(Renderer& renderer, color old_buffer[], color new_buffer[], int samples_per_pixel) {
  double weight = 1.0 / (renderer.frame_count() + 1.0);
  for (int j = renderer.get_height()-1; j >= 0; j--) {
    for (int i = 0; i < renderer.get_width(); i++) {
      int index = (j * renderer.get_width()) + i;
      new_buffer[index] = set_color(new_buffer[index], samples_per_pixel);
      new_buffer[index] = old_buffer[index] * (1 - weight) + new_buffer[index] * weight;
      renderer.set_pixel(i, j, new_buffer[index].x(), new_buffer[index].y(), new_buffer[index].z());
    }
  }  

}
