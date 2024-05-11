#include "headers/render.h"
#include "headers/extras.h"

#include "headers/util.h"
#include "headers/color.h"
#include "headers/hittable_list.h"
#include "headers/sphere.h"
#include "headers/camera.h"
#include "headers/vec3.h"
#include "imgui.h"

// Image Constants
const auto TEX_ASPECT = 16.0 / 9.0;
const int TEX_WIDTH = 1000;
const int TEX_HEIGHT = static_cast<int>(TEX_WIDTH / TEX_ASPECT);

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

    for (int j = TEX_HEIGHT-1; j >= 0; j--) {
        for (int i = 0; i < TEX_WIDTH; i++) {
            color pixel_color(0, 0, 0);
            uint pixel_coord = (j * TEX_WIDTH-1) + i;
            for (int s = 0; s < samples_per_pixel; ++s) {
                auto u = (i + pcg_random_double(pixel_coord)) / (TEX_WIDTH-1);
                auto v = (j + pcg_random_double(pixel_coord)) / (TEX_HEIGHT-1);
                ray r = cam.get_ray(u, v);
                pixel_color += pcg_ray_color(r, world, max_depth, pixel_coord);
            }
    
            write_color(renderer, pixel_color, samples_per_pixel, i, j);
        }
    }

}
// -----------------------------------------------------------------------------
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

void render_accumulate(Renderer &renderer, hittable_list& world, camera& cam, Parameters& parameters) {

    int samples_per_pixel = 1;
    const int max_depth = 50;

    if (parameters.switched && parameters.render_type == 2) {
        renderer.clear_buffer(parameters.new_buffer);
        renderer.reset_frame_count();
        samples_per_pixel = 2;
        parameters.switched = false;
    } else if (parameters.switched && parameters.render_type == 0) {
        samples_per_pixel = 1;
        parameters.switched = false;
    }

    if (parameters.render_type == 2) {
        memcpy(parameters.old_buffer, parameters.new_buffer, sizeof(color) * parameters.tex_width*parameters.tex_height);
    }
    for (int j = TEX_HEIGHT-1; j >= 0; j--) {
        for (int i = 0; i < TEX_WIDTH; i++) {
            color pixel_color(0, 0, 0);
            for (int s = 0; s < samples_per_pixel; ++s) {
                auto u = (i + random_double()) / (TEX_WIDTH-1);
                auto v = (j + random_double()) / (TEX_HEIGHT-1);
                ray r = cam.get_ray(u, v);
                pixel_color += ray_color(r, world, max_depth);
            }
            if (parameters.render_type == 2) {
                parameters.new_buffer[(j * TEX_WIDTH) + i] = pixel_color;
            } else if (parameters.render_type == 0) {
                write_color(renderer, pixel_color, samples_per_pixel, i, j);       
            }
        }
    }
    if (parameters.render_type == 2)
        accumulate(renderer, parameters.old_buffer, parameters.new_buffer, samples_per_pixel);
}
// -----------------------------------------------------------------------------

int main() {

    // Renderer
    int window_width = 1200;
    int window_height = 635;
    Renderer renderer(window_width, window_height, TEX_WIDTH, TEX_HEIGHT);
    Parameters parameters(TEX_WIDTH, TEX_HEIGHT);

    // World
    hittable_list world;
    auto sphere1 = make_shared<sphere>(point3(-0.25,0,-1), 0.5);
    auto sphere2 = make_shared<sphere>(point3(0.25,0,-1), 0.5);
    auto controlled_sphere = sphere1;
    world.add(sphere2);
    world.add(sphere1);

    // Camera 
    camera cam(TEX_ASPECT);

    int sphere_toggle = 1;

    // Render Loop
    while(true) {

        handle_inputs(renderer.input(), *controlled_sphere, parameters);

        renderer.begin_new_frame();

        ImGui::Begin("Info");

        if (ImGui::Button("Regular")) {
            parameters.render_type = 0;
            parameters.switched = true;
        }

        if (ImGui::Button("Seeded")) {
            parameters.render_type = 1;
        }

        if (ImGui::Button("Accumulate")) {
            parameters.render_type = 2;
            parameters.switched = true;
        }

        switch (parameters.render_type) {
            case 1: {
                // Renderer using seeded randomness. Seed is the pixel coordinate and results in the
                // same number each time (for that seed)
                ImGui::Text("PCG render");
                auto t1 = NOW();
                pcg_render(renderer, world, cam);
                auto t2 = NOW();
                double elapsed = GET_TIME(t2, t1);
                ImGui::Text("%f FPS", 1 / elapsed);
                break;
            }
            default: {
                // Uses the regular renderer, but each pixel color is an average of the rays sent
                // through it. Accumulates over time
                ImGui::Text("regular render");
                auto t1 = NOW();
                render_accumulate(renderer, world, cam, parameters);
                auto t2 = NOW();
                double elapsed = GET_TIME(t2, t1);
                ImGui::Text("%f FPS", 1 / elapsed);
                break;
            }
        }

        if (ImGui::Button("toggle sphere"))
            sphere_toggle *= -1;

        if (sphere_toggle == 1) {
            controlled_sphere = sphere1;
            ImGui::Text("Controlling Sphere1");
        } else if (sphere_toggle == -1) {
            controlled_sphere = sphere2;
            ImGui::Text("Controlling Sphere2");
        }

        ImGui::End();

        // WASD to move one of the spheres
        // Q for the regular renderer
        // E for seeded randomness
        // R to accumulate over time

        renderer.present();
    }
  
  
    return 0;
}

