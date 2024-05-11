#pragma once

#include <cstring>
#include <iostream>

#include "../../lib/SDL2/include/SDL.h"
#include "../../lib/imgui/imgui.h"
#include "../../lib/imgui/backends/imgui_impl_sdl2.h"
#include "../../lib/imgui/backends/imgui_impl_sdlrenderer2.h"

#define ASSERT(_e, ...) if (!_e) { fprintf(stderr, __VA_ARGS__); exit(1); }

class Renderer {
public:
    Renderer(int window_width, int window_height, int texture_width, int texture_height)
        : window_width(window_width), window_height(window_height), tex_width(texture_width), tex_height(texture_height) {
        ASSERT(
            !SDL_Init(SDL_INIT_VIDEO),
            "SDL failed to initialize: %s\n",
            SDL_GetError());

        this->window = SDL_CreateWindow(
            "Test", 
            SDL_WINDOWPOS_CENTERED, 
            SDL_WINDOWPOS_CENTERED,
            window_width, 
            window_height, 
            SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI
        );
        ASSERT(this->window, "Window failed to load: %s\n", SDL_GetError());

        this->renderer = SDL_CreateRenderer(
            this->window, 
            -1, 
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
        );
        ASSERT(this->renderer, "Renderer failed to load: %s\n", SDL_GetError());

        this->texture = SDL_CreateTexture(
            this->renderer, 
            SDL_PIXELFORMAT_ABGR8888, 
            SDL_TEXTUREACCESS_STREAMING, 
            this->tex_width, 
            this->tex_height
        );
        ASSERT(this->texture, "Texture didn't work, idk: %s\n", SDL_GetError());

        this->format = SDL_AllocFormat(SDL_PIXELFORMAT_ABGR8888);

        this->pixels = new uint[this->tex_width * this->tex_height] {0};

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        this->io = &ImGui::GetIO();
        this->io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        this->io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        this->io->ConfigViewportsNoAutoMerge = true;
        //io.ConfigViewportsNoTaskBarIcon = true;

        ImGui::StyleColorsDark();
        ImGui_ImplSDL2_InitForSDLRenderer(this->window, this->renderer);
        ImGui_ImplSDLRenderer2_Init(this->renderer);
    }

    ~Renderer() {
        delete this->pixels;
        ImGui_ImplSDLRenderer2_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
        SDL_FreeFormat(this->format);
        SDL_DestroyTexture(this->texture);
        SDL_DestroyRenderer(this->renderer);
        SDL_DestroyWindow(this->window);
        SDL_Quit();
    }

    void present() {
        SDL_UpdateTexture(this->texture, NULL, this->pixels, this->tex_width * sizeof(uint));

        ImGui::Begin("Texture");
        ImGui::Image(reinterpret_cast<ImTextureID>(this->texture), ImVec2(this->tex_width, this->tex_height));
        ImGui::End();

        ImGui::End();



        ImGui::Render();
        ImGuiIO &io = ImGui::GetIO();
        SDL_RenderSetScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            std::cout << "hi" << std::endl;
            SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
            SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
        }
        // SDL_RenderCopyEx(
        //     this->renderer, 
        //     this->texture,  
        //     NULL,
        //     NULL,
        //     0.0,
        //     NULL,
        //     SDL_FLIP_VERTICAL);
        SDL_RenderPresent(this->renderer);
        this->num_frames++;
    }

    void begin_new_frame() {
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(this->window_width, this->window_height));

        ImGui::Begin("Workspace");

        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
    }

    int get_window_width() { return this->window_width; }
    int get_window_height() { return this->window_height; }
    int get_texture_width() { return this->tex_width; }
    int get_texture_height() { return this->tex_height; }
    int frame_count() { return this->num_frames; }
    void reset_frame_count() { this->num_frames = 0; }

public:
    const Uint8* input() {
        const Uint8 *keystates = SDL_GetKeyboardState(NULL);

        while(SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT) {
                //std::cout << "\nClosing program" << std::endl;
                SDL_Quit();
                exit(0);
            }
        }
        return keystates;
    }

    void set_pixel(int x, int y, uint color) {
        this->pixels[(y * this->tex_width) + x] = color;
    }

    void set_pixel(int x, int y, uint r, uint g, uint b) {
        uint color = SDL_MapRGB(this->format, r, g, b);
        set_pixel(x, y, color);
    }

    void clear_buffer() {
        memset(this->pixels, 0, (this->tex_width * this->tex_height) * sizeof(uint));
    }

    template<class C>
    void clear_buffer(C *buffer) {
        memset(buffer, 0, (this->tex_width * this->tex_height) * sizeof(*buffer));
    }


  
private:
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    SDL_Texture *texture = nullptr;
    SDL_Event event;
    SDL_PixelFormat *format;

    uint *pixels;
    int window_width, window_height;
    int tex_width, tex_height;

    ImGuiIO *io;

    int num_frames = 0;
};
