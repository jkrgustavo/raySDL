#pragma once

#include <cstring>
#include <iostream>

#include "../../lib/SDL2/include/SDL.h"


#define ASSERT(_e, ...) if (!_e) { fprintf(stderr, __VA_ARGS__); exit(1); }

class Renderer {
public:
  Renderer(int screen_width, int screen_height, int window_width, int window_height)
    : screen_width(screen_width), screen_height(screen_height) {
    ASSERT(
        !SDL_Init(SDL_INIT_VIDEO),
        "SDL failed to initialize: %s\n",
        SDL_GetError());

    this->window = 
      SDL_CreateWindow(
        "Test",
        SDL_WINDOWPOS_CENTERED,
      SDL_WINDOWPOS_CENTERED,
      window_width,
      window_height,
      SDL_WINDOW_ALLOW_HIGHDPI);
    ASSERT(this->window, "Window failed to load: %s\n", SDL_GetError());

  this->renderer =
    SDL_CreateRenderer(
      this->window,
      -1, 
      SDL_RENDERER_PRESENTVSYNC);
  ASSERT(this->renderer, "Renderer failed to load: %s\n", SDL_GetError());

  this->texture = 
    SDL_CreateTexture(
      this->renderer,
      SDL_PIXELFORMAT_ABGR8888, 
      SDL_TEXTUREACCESS_STREAMING, 
      this->screen_width,
      this->screen_height);
  ASSERT(this->texture, "Texture didn't work, idk: %s\n", SDL_GetError());

  this->format = SDL_AllocFormat(SDL_PIXELFORMAT_ABGR8888);

  this->pixels = new uint[this->screen_width * this->screen_height] {0};
}

~Renderer() {
  delete this->pixels;
  SDL_FreeFormat(this->format);
  SDL_DestroyTexture(this->texture);
  SDL_DestroyRenderer(this->renderer);
  SDL_DestroyWindow(this->window);
  SDL_Quit();
}

void present() {
  SDL_UpdateTexture(this->texture, NULL, this->pixels, this->screen_width * 4);
  SDL_RenderCopyEx(
      this->renderer, 
      this->texture,  
      NULL,
      NULL,
      0.0,
      NULL,
      SDL_FLIP_VERTICAL);
  SDL_RenderPresent(this->renderer);
  this->num_frames++;
}

int get_width() { return this->screen_width; }
int get_height() { return this->screen_height; }
int frame_count() { return this->num_frames; }
void reset_frame_count() { this->num_frames = 0; }

public:
  const Uint8* input() {
    const Uint8 *keystates = SDL_GetKeyboardState(NULL);

    while(SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        //std::cout << "\nClosing program" << std::endl;
        SDL_Quit();
        exit(0);
      }

    //if (keystates[SDL_SCANCODE_Q]) {}
    //if (keystates[SDL_SCANCODE_E]) {}

    //if (keystates[SDL_SCANCODE_W]) {}
    //if (keystates[SDL_SCANCODE_A]) {}
    //if (keystates[SDL_SCANCODE_S]) {}
    //if (keystates[SDL_SCANCODE_D]) {}

    //if (keystates[SDL_SCANCODE_ESCAPE]) {}

    //if (keystates[SDL_SCANCODE_Z]) {}
    //if (keystates[SDL_SCANCODE_X]) {}

    }
    return keystates;
  }

  void set_pixel(int x, int y, uint color) {
    this->pixels[(y * this->screen_width) + x] = color;
  }

  void set_pixel(int x, int y, uint r, uint g, uint b) {
    uint color = SDL_MapRGB(this->format, r, g, b);
    set_pixel(x, y, color);
  }

  void clear_buffer() {
    memset(this->pixels, 0, (this->screen_width * this->screen_height) * 4);
  }

  template<class C>
  void clear_buffer(C *buffer) {
    memset(buffer, 0, (this->screen_width * this->screen_height) * sizeof(*buffer));
  }


  
private:
  SDL_Window *window = nullptr;
  SDL_Renderer *renderer = nullptr;
  SDL_Texture *texture = nullptr;
  SDL_Event event;
  SDL_PixelFormat *format;

  uint *pixels;
  int screen_width, screen_height;

  int num_frames = 0;

public:
};
