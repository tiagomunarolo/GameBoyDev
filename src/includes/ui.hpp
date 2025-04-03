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

static unsigned long COLORS[4] = {WHITE, LIGHT_GRAY, DARK_GRAY, BLACK};

static SDL_Window *window;
static SDL_Renderer *renderer;
static SDL_Surface *screen;
static SDL_Texture *texture;
#ifdef DEBUG_UI
static SDL_Window *debug_window;
static SDL_Renderer *debug_renderer;
static SDL_Surface *debug_screen;
static SDL_Texture *debug_texture;
#endif

class UI
{
private:
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