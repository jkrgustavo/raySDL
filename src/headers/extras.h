#pragma once

#include "../../lib/SDL2/include/SDL.h"

#include "util.h"
#include "sphere.h"

struct Parameters {
  Parameters(int width, int height) 
    : screen_width(width), screen_height(height) {}
  ~Parameters() {
    delete[] old_buffer;
    delete[] new_buffer; 
  }

  int screen_width, screen_height;
  color *old_buffer = new color[screen_width*screen_height];
  color *new_buffer = new color[screen_width*screen_height];

  bool switched = false;
  int render_type = 0;
};

void handle_inputs(const Uint8* keystates, sphere &sphere, Parameters& parameters) {
  if (keystates[SDL_SCANCODE_W]) { sphere.center[2] -= 0.1; }
  if (keystates[SDL_SCANCODE_A]) { sphere.center[0] -= 0.2; }
  if (keystates[SDL_SCANCODE_S]) { sphere.center[2] += 0.1; }
  if (keystates[SDL_SCANCODE_D]) { sphere.center[0] += 0.2; }

  if (keystates[SDL_SCANCODE_Q]) { parameters.render_type = 0; }
  if (keystates[SDL_SCANCODE_E]) { parameters.render_type = 1; }
  if (keystates[SDL_SCANCODE_R]) { parameters.render_type = 2; parameters.switched = true; }

  if (keystates[SDL_SCANCODE_ESCAPE]) { SDL_Quit(); exit(0); }
}

void intersect_spheres(sphere &small, sphere &surface) {
  auto tether = small.center - surface.center;
  double distance = tether.length();
  if (distance >= (surface.radius + small.radius)) {
    small.center[1] -= (distance - (surface.radius + small.radius));
  }
  if (distance <= (surface.radius + small.radius)) {
    small.center[1] += ((surface.radius + small.radius) - distance);
  }
}

