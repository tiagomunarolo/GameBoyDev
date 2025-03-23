#pragma once
#include <iostream>
#include "definitions.hpp"
#include "memory.hpp"
#include "ppu.hpp"
#include <SDL.h>
#include <SDL_ttf.h>

// Screen dimensions
const int SCREEN_WIDTH_DEFAULT = 160;
const int SCREEN_HEIGHT_DEFAULT = 144;

class UI
{
private:
    const int w = SCREEN_WIDTH_DEFAULT;
    // const int h = SCREEN_HEIGHT_DEFAULT;
    //  windows
    SDL_Window *window;
    SDL_Window *debug_window;
    // renderer
    SDL_Renderer *renderer;
    SDL_Renderer *debug_renderer;
    // surface
    SDL_Surface *screen;
    // texture
    SDL_Texture *texture;
    SDL_Texture *debug_texture;
    Memory *mem;
    PixelProcessingUnit *ppu;
    void update_window();
#ifdef DEBUG_UI
    void update_dbg_window();
#endif
    void sdl_init();

public:
    UI(Memory *mem, PixelProcessingUnit *ppu);
    bool running = false; // running flag
    void update();
    void check_event();
    void quit();
};

extern UI *ui;