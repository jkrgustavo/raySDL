#include <iostream>
#include <chrono>

//#include "../lib/SDL2-2.30.3/include/SDL.h"

#include "headers/render.h"
#include "headers/extras.h"

#include "headers/util.h"
#include "headers/color.h"
#include "headers/hittable_list.h"
#include "headers/sphere.h"
#include "headers/camera.h"
#include "headers/vec3.h"

// Image Constants
const auto aspect_ratio = 16.0 / 9.0;
const int SCREEN_WIDTH = 500;
const int SCREEN_HEIGHT = static_cast<int>(SCREEN_WIDTH / aspect_ratio);

using namespace std::chrono;

// --------------------------------------PCG-------------------------------------
color pcg_ray_color(const ray& r, const hittable& world, int depth, uint seed) {
  hit_record rec;
  if (depth <= 0)
    return color(0,0,0);

  if (world.hit(r, 0.001, infinity, rec)) {
    point3 target = rec.p + rec.normal + pcg_in_unit_sphere(seed);
    return 0.5 * pcg_ray_color(ray(rec.p, target - rec.p), world, depth-1, seed);
  }
  vec3 unit_direction = unit_vector(r.direction());
  auto t = 0.5*(unit_direction.y() + 1.0);
  return (1.0-t)*color(1.0, 1.0, 1.0) + t*color(0.5, 0.7, 1.0);
}

void pcg_render(Renderer &renderer, hittable_list& world, camera& cam) {
  renderer.clear_buffer();

  const int samples_per_pixel = 1;
  const int max_depth = 50;

  for (int j = SCREEN_HEIGHT-1; j >= 0; j--) {
    for (int i = 0; i < SCREEN_WIDTH; i++) {
      color pixel_color(0, 0, 0);
      uint pixel_coord = (j * SCREEN_WIDTH-1) + i;
      for (int s = 0; s < samples_per_pixel; ++s) {
        auto u = (i + pcg_random_double(pixel_coord)) / (SCREEN_WIDTH-1);
        auto v = (j + pcg_random_double(pixel_coord)) / (SCREEN_HEIGHT-1);
        ray r = cam.get_ray(u, v);
        pixel_color += pcg_ray_color(r, world, max_depth, pixel_coord);
      }
    
      write_color(renderer, pixel_color, samples_per_pixel, i, j);
    }
  }

}
// --------------------------------Regular--------------------------------------
color ray_color(const ray& r, const hittable& world, int depth) {
  hit_record rec;
  if (depth <= 0)
    return color(0,0,0);

  if (world.hit(r, 0.001, infinity, rec)) {
    point3 target = rec.p + rec.normal + random_in_unit_sphere();
    return 0.5 * ray_color(ray(rec.p, target - rec.p), world, depth-1);
  }
  vec3 unit_direction = unit_vector(r.direction());
  auto t = 0.5*(unit_direction.y() + 1.0);
  return (1.0-t)*color(1.0, 1.0, 1.0) + t*color(0.5, 0.7, 1.0);
}

void render(Renderer &renderer, hittable_list& world, camera& cam) {
  renderer.clear_buffer();

  const int samples_per_pixel = 1;
  const int max_depth = 50;

  for (int j = SCREEN_HEIGHT-1; j >= 0; j--) {
    //std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
    for (int i = 0; i < SCREEN_WIDTH; i++) {
      color pixel_color(0, 0, 0);
      for (int s = 0; s < samples_per_pixel; ++s) {
        auto u = (i + random_double()) / (SCREEN_WIDTH-1);
        auto v = (j + random_double()) / (SCREEN_HEIGHT-1);
        ray r = cam.get_ray(u, v);
        pixel_color += ray_color(r, world, max_depth);
      }
      write_color(renderer, pixel_color, samples_per_pixel, i, j);
    }
  }
}
// ----------------------------------Accumulate----------------------------------

void render_accumulate(Renderer &renderer, hittable_list& world, camera& cam, Parameters& parameters) {

  const int samples_per_pixel = 2;
  const int max_depth = 50;

  if (parameters.switched) {
    renderer.clear_buffer(parameters.new_buffer);
    renderer.reset_frame_count();
    parameters.switched = false;
  }
  memcpy(parameters.old_buffer, parameters.new_buffer, sizeof(*(parameters.new_buffer))*SCREEN_WIDTH*SCREEN_HEIGHT);
  for (int j = SCREEN_HEIGHT-1; j >= 0; j--) {
      for (int i = 0; i < SCREEN_WIDTH; i++) {
        color pixel_color(0, 0, 0);
        int index = (j * SCREEN_WIDTH) + i;
        for (int s = 0; s < samples_per_pixel; ++s) {
          auto u = (i + random_double()) / (SCREEN_WIDTH-1);
          auto v = (j + random_double()) / (SCREEN_HEIGHT-1);
          ray r = cam.get_ray(u, v);
          pixel_color += ray_color(r, world, max_depth);
        }
        parameters.new_buffer[index] = pixel_color;
      }
    }
  accumulate(renderer, parameters.old_buffer, parameters.new_buffer, samples_per_pixel);
}
// -----------------------------------------------------------------------------

int main() {

  // Renderer
  int window_width = 1000;
  int window_height = static_cast<int>(window_width / aspect_ratio);
  Renderer renderer(SCREEN_WIDTH, SCREEN_HEIGHT, window_width, window_height);
  Parameters parameters(SCREEN_WIDTH, SCREEN_HEIGHT);

  // World
  hittable_list world;
  auto sphere1 = make_shared<sphere>(point3(-0.25,0,-1), 0.5);
  auto sphere2 = make_shared<sphere>(point3(0.25,0,-1), 0.5);
  world.add(sphere2);
  world.add(sphere1);

  // Camera 
  camera cam(aspect_ratio);

  // Render Loop
  while(true) {

    switch (parameters.render_type) {
      case 0: {
        // Regular rendering, one ray per pixel, with some randomness intended for sampling
        auto t1 = high_resolution_clock::now();
        render(renderer, world, cam);
        auto t2 = high_resolution_clock::now();
        duration<double> elapsed = duration_cast<duration<double>>(t2 - t1);
        std::cout << "\rRender Time: " << elapsed.count() << " seconds" << std::flush;
        break;
      }
      case 1: {
        // Renderer using seeded randomness. Seed is the pixel coordinate and results in the
        // same number each time (for that seed)
        auto t1 = high_resolution_clock::now();
        pcg_render(renderer, world, cam);
        auto t2 = high_resolution_clock::now();
        duration<double> elapsed = duration_cast<duration<double>>(t2 - t1);
        std::cout << "\rRender Time: " << elapsed.count() << " seconds" << std::flush;
        break;
      }
      case 2: {
        // Uses the regular renderer, but each pixel color is an average of the rays sent
        // through it. Accumulates over time
        auto t1 = high_resolution_clock::now();
        render_accumulate(renderer, world, cam, parameters);
        auto t2 = high_resolution_clock::now();
        duration<double> elapsed = duration_cast<duration<double>>(t2 - t1);
        std::cout << "\rRender Time: " << elapsed.count() << " seconds" << std::flush;
        break;
      }
    }

    // WASD to move one of the spheres
    // Q for the regular renderer
    // E for the seeded randomness
    // R to accumulate over time
    handle_inputs(renderer.input(), *sphere1, parameters);

    renderer.present();
  }
  
  
  return 0;
}

