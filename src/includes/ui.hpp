#pragma once
#include <iostream>
#include "definitions.hpp"
#include <SDL.h>
#include <SDL_ttf.h>

// Screen dimensions
const int SCREEN_WIDTH_DEFAULT = 160;
const int SCREEN_HEIGHT_DEFAULT = 144;

// Color definitios
static unsigned long WHITE = 0xFFFFFFFF;
static unsigned long LIGHT_GRAY = 0xFFAAAAAA;
static unsigned long DARK_GRAY = 0xFF555555;
static unsigned long BLACK = 0xFF000000;
const int SCALE = 3;

static unsigned long tile_colors[4] = {WHITE, LIGHT_GRAY, DARK_GRAY, BLACK};

class UI
{
private:
    //  windows, renderers and surfaces
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Surface *screen;
    SDL_Texture *texture;
#ifdef DEBUG_UI
    SDL_Window *debug_window;
    SDL_Renderer *debug_renderer;
    SDL_Surface *debug_screen;
    SDL_Texture *debug_texture;
#endif
#ifdef DEBUG_UI
    void update_dbg_window();
#endif
    void sdl_init();

public:
    UI();
    bool running = false; // running flag
    void update(bool render);
    void check_event();
    void quit();
};

extern UI *ui;