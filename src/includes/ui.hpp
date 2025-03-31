#pragma once
#include <iostream>
#include "definitions.hpp"
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
    void renderObjs();       // render OAM sprites if required
    void renderWindow();     // render Window Layer
    void renderBackGround(); // Render background
#ifdef DEBUG_UI
    void update_dbg_window();
#endif
    void sdl_init();

public:
    UI();
    bool running = false; // running flag
    void update();
    void check_event();
    void quit();
};

extern UI *ui;