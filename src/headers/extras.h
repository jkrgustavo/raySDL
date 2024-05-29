#pragma once

#include "../../lib/SDL2/include/SDL.h"

#include "util.h"
#include "sphere.h"

struct Parameters {
    Parameters(int width, int height) 
        : tex_width(width), tex_height(height) {}
    ~Parameters() {
        delete[] buffer; 
    }

    int tex_width, tex_height;
    uint *buffer = new uint[tex_width*tex_height];
    color *color_buffer = new color[tex_width*tex_height];

    bool switched = false;
    int render_type = 0;
};

void handle_inputs(const Uint8* keystates, shared_ptr<sphere> sphere, Parameters& parameters, simd::double1 dt) {
    if (keystates[SDL_SCANCODE_W]) { sphere->center[2] -= 1 * dt; }
    if (keystates[SDL_SCANCODE_A]) { sphere->center[0] -= 1 * dt; }
    if (keystates[SDL_SCANCODE_S]) { sphere->center[2] += 1 * dt; }
    if (keystates[SDL_SCANCODE_D]) { sphere->center[0] += 1 * dt; }

    if (keystates[SDL_SCANCODE_Q]) { parameters.render_type = 0; }
    if (keystates[SDL_SCANCODE_E]) { parameters.render_type = 1; }
    if (keystates[SDL_SCANCODE_R]) { parameters.render_type = 2; parameters.switched = true; }

    if (keystates[SDL_SCANCODE_ESCAPE]) { SDL_Quit(); exit(0); }
}

// Keeps the surface of one sphere on the other, wasd keys make one sphere circle the other
void intersect_spheres(shared_ptr<sphere> small, shared_ptr<sphere> surface) {
    auto tether = small->center - surface->center;
    double distance = simd::length(tether);
    if (distance >= (surface->radius + small->radius)) {
        small->center[1] -= (distance - (surface->radius + small->radius));
    }
    if (distance <= (surface->radius + small->radius)) {
        small->center[1] += ((surface->radius + small->radius) - distance);
    }
}

