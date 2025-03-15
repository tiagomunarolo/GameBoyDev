#pragma once
#include <iostream>
#include "definitions.hpp"
#include <SDL.h>
#include <SDL_ttf.h>

// Screen dimensions
const int SCREEN_WIDTH_DEFAULT = 800;
const int SCREEN_HEIGHT_DEFAULT = 600;

class UI
{
public:
    bool running = false;
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Color color;
    SDL_Texture *texture;
    const int SCREEN_WIDTH = SCREEN_WIDTH_DEFAULT;
    const int SCREEN_HEIGHT = SCREEN_HEIGHT_DEFAULT;
    TTF_Font *font;
    UI();
    void quit();
    void renderText(const std::string &text);
};