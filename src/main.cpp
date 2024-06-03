#include "headers/render.h"
#include "headers/extras.h"

#include "headers/thread.h"
#include "headers/util.h"
#include "headers/color.h"
#include "headers/hittable_list.h"
#include "headers/sphere.h"
#include "headers/camera.h"
#include "headers/vec3.h"
#include "headers/material.h"
#include <imgui.h>

// Image Constants
const auto TEX_ASPECT = 16.0 / 9.0;
const int TEX_WIDTH = 1000;
const int TEX_HEIGHT = static_cast<int>(TEX_WIDTH / TEX_ASPECT);

void sphere_menu(Scene &scene, Parameters &params, double dt);
void thread_menu(int thread_count, std::vector<RenderTask> task_collection);

// --------------------------------------PCG-------------------------------------
color pcg_ray_color(const ray& r, const hittable& world, int depth, uint seed) {
    hit_record rec;
    if (depth <= 0)
        return simd::make_float3(0,0,0);

    if (world.hit(r, 0.001, infinity, rec)) {
        ray scattered;
        color attenuation;
        if (rec.mat->scatter(r, rec, attenuation, scattered, seed)) {
            return attenuation * pcg_ray_color(scattered, world, depth-1, seed);
        }
        return simd::make_float3(0, 0, 0);
    }
    vec3 unit_direction = simd::normalize(r.direction());
    auto t = 0.5*(unit_direction.y + 1.0);
    return (1.0-t)*simd::make_float3(1.0, 1.0, 1.0) + t*simd::make_float3(0.5, 0.7, 1.0);
}

void pcg_render(hittable_list& world, camera& cam, Parameters &parameters, RenderTask task) {
    const int max_depth = 50;

    for (int j = task.start_y; j < task.end_y; j++) {
        for (int i = task.start_x; i < task.end_x; i++) {
            color pixel_color = simd::make_float3(0, 0, 0);
            simd::uint1 pixel_coord = (j * TEX_WIDTH-1) + i;
            for (int s = 0; s < parameters.samples_per_pixel; ++s) {
                auto u = (i + pcg_random_float(pixel_coord)) / (TEX_WIDTH-1);
                auto v = (j + pcg_random_float(pixel_coord)) / (TEX_HEIGHT-1);
                ray r = cam.get_ray(u, v);
                pixel_color += pcg_ray_color(r, world, max_depth, pixel_coord);
            }
            parameters.color_buffer[j * parameters.tex_width + i] = pixel_color;
        }
    }
}
// -----------------------------------------------------------------------------
color ray_color(const ray& r, const hittable& world, int depth) {
    hit_record rec;
    if (depth <= 0)
        return simd::make_float3(0, 0, 0);

    if (world.hit(r, 0.001, infinity, rec)) {
        ray scattered;
        color attenuation;
        if (rec.mat->scatter(r, rec, attenuation, scattered)) {
            return attenuation * ray_color(scattered, world, depth-1);
        }
        return simd::make_float3(0, 0, 0);
    }

    vec3 unit_direction = simd::normalize(r.direction());
    auto t = 0.5*(unit_direction.y + 1.0);
    return (1.0-t)*simd::make_float3(1.0, 1.0, 1.0) + t*simd::make_float3(0.5, 0.7, 1.0);
}

void render(hittable_list& world, camera& cam, Parameters& parameters, RenderTask task) {

    const int max_depth = 10;

    for (int j = task.start_y; j < task.end_y; j++) {
        for (int i = task.start_x; i < task.end_x; i++) {
            color pixel_color = simd::make_float3(0, 0, 0);;
            for (int s = 0; s < parameters.samples_per_pixel; ++s) {
                auto u = (i + random_float()) / (TEX_WIDTH-1);
                auto v = (j + random_float()) / (TEX_HEIGHT-1);
                ray r = cam.get_ray(u, v);
                pixel_color += ray_color(r, world, max_depth);
            }
            parameters.color_buffer[j * parameters.tex_width + i] = pixel_color;
        }
    }
}
// -----------------------------------------------------------------------------

void thread_render(ThreadManager &threads, hittable_list &world, camera &cam, Parameters &params) {
    while (true) {
        RenderTask task;
        threads.task_queue.wait_and_pop(task);

        pcg_render(world, cam, params, task);

        threads.completion_queue.push(task);
    }
}


int main() {

    // Renderer
    int window_width = 1300;
    int window_height = 635;
    Renderer renderer(window_width, window_height, TEX_WIDTH, TEX_HEIGHT);
    Parameters parameters(TEX_WIDTH, TEX_HEIGHT);
    parameters.render_type = 2;

    Scene scene;
    scene.init_scene1();

    // Camera 
    camera cam(TEX_ASPECT);

    ThreadManager threads(TEX_WIDTH, TEX_HEIGHT);
    threads.threads_init(thread_render, std::ref(threads), std::ref(scene.world), std::ref(cam), std::ref(parameters));

    auto time = NOW();

    // Render Loop
    while(true) {
        double dt = GET_TIME(NOW(), time);
        time = NOW();

        handle_inputs(renderer.input(), std::static_pointer_cast<sphere>(scene.controlled), parameters, dt);

        renderer.begin_new_frame();

        switch (parameters.render_type) {
            case 1: {
                render(scene.world, cam, parameters, RenderTask {
                    .start_x = 0,
                    .start_y = 0,
                    .end_x = TEX_WIDTH,
                    .end_y = (uint)TEX_HEIGHT,
                });
                break;
            }
            case 2: {
                pcg_render(scene.world, cam, parameters, RenderTask {
                    .start_x = 0,
                    .start_y = 0,
                    .end_x = TEX_WIDTH,
                    .end_y = (uint)TEX_HEIGHT,
                });
                break;
            }
            default: {
                threads.push_tasks();
            }
        }

       if (parameters.render_type != 1 && parameters.render_type != 2)
            threads.wait_for_completion();

        sphere_menu(scene, parameters, dt);
        thread_menu(threads.thread_count, threads.task_collection);

        fast_color_pack(parameters.color_buffer, parameters.buffer, parameters.samples_per_pixel, TEX_WIDTH * TEX_HEIGHT);
        renderer.set_buffer(parameters.buffer);
        renderer.present();
    }
  
  
    return 0;
}

void sphere_menu(Scene &scene, Parameters &params, double dt) {
    int i = 0;
    static int sphere_toggle = 1;
    static int color_toggle = -1;
    static float col[] = { 0.0, 0.0, 0.0 };

    ImGui::Begin("Info");
    ImGui::Text("%.2f FPS", 1 / dt);
    if (ImGui::Button("multi")) {
        params.render_type = 0;
    }
    ImGui::SameLine();
    if (ImGui::Button("regular")) {
        params.render_type = 1;
    }
    ImGui::SameLine();
    if (ImGui::Button("PCG")) {
        params.render_type = 2;
    }
    ImGui::SameLine();
    if (params.render_type == 1) {
        ImGui::Text("Default Render");
    } else if (params.render_type == 2) {
        ImGui::Text("PCG Renderer");
    } else {
        ImGui::Text("Multi-threaded");
    }

    if (ImGui::Button("toggle sphere")) {
        scene.toggle_controlled();
    }
    std::string label_sphere = "Controlled: sphere" + std::to_string(sphere_toggle);
    ImGui::SameLine();
    ImGui::Text("%s", label_sphere.c_str());

    for (shared_ptr<hittable> object : scene.world.objects) {
        std::shared_ptr<sphere> s = std::static_pointer_cast<sphere>(object);
        std::string label = "Sphere " + std::to_string(i) + "##" + std::to_string(i);

        if (ImGui::CollapsingHeader(label.c_str())) {
            ImGui::Text(
                "Center:\n x: %.2f, y: %.2f, z: %.2f", 
                s->center.x, s->center.y, s->center.z
            );
            ImGui::Text(
                "Color:\n r: %.2f, g: %.2f, b: %.2f", 
                s->mat->get_color().x, s->mat->get_color().y, s->mat->get_color().z
            );
            ImGui::Text("Material:\n %s", s->mat->type_name());

            if (ImGui::Button("color")) {
                color_toggle *= -1;
            }
            ImGui::SameLine();
            if (ImGui::Button("delete")) {
                scene.world.objects.erase(scene.world.objects.begin() + i);
            }
            ImGui::SameLine();
            if (ImGui::Button("metal")) {
                s->mat = make_shared<metal>(simd::make_float3(0.3, 0.3, 0.3));
            }
            ImGui::SameLine();
            if (ImGui::Button("lambertian")) {
                s->mat = make_shared<lambertian>(simd::make_float3(0.3, 0.3, 0.3));
            }

            if (color_toggle == 1) {
                ImGui::ColorPicker3("Sphere Color", col);
                s->mat->set_color(simd::make_float3(col[0], col[1], col[2]));
            }
        }

        i++;
    }

    if (ImGui::Button("New Sphere")) {
        auto material_center = make_shared<lambertian>(simd::make_float3(0.5, 0.5, 0.5));
        scene.world.add(make_shared<sphere>(simd::make_float3(0.0, 0.0, -1.2), 0.5, material_center));
    }
    ImGui::End();
}

void thread_menu(int thread_count, std::vector<RenderTask> task_collection) {
        ImGui::Begin("ThreadInfo");
        ImGui::Text("Current threads: %d", thread_count);
        ImGui::Text("Task count: %zu", task_collection.size());
        if (ImGui::CollapsingHeader("TaskInfo"))
            for (int i = 0; i < task_collection.size(); i++) {
                RenderTask task = task_collection[i];
                std::string task_label = "Task" + std::to_string(i);
                ImGui::Text("%s", task_label.c_str());   
                ImGui::SameLine();
                ImGui::Text("x: %d, y: %d, u: %d, v: %d", task.start_x, task.start_y, task.end_x, task.end_y);
            }
        ImGui::End();
}
