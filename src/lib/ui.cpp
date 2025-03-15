#include "ui.hpp"

using namespace std;

// intialize SDL and SDL_ttf. Returns SDL_Window
static SDL_Window *sdl_init()
{
    // Initialize SDL and SDL_ttf
    if (TTF_Init() == -1)
    {
        std::cerr << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << std::endl;
        exit(1);
    }
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        exit(1);
    }
    // Create window (SDL_Window)
    return SDL_CreateWindow("GB Emulator",
                            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                            SCREEN_WIDTH_DEFAULT, SCREEN_HEIGHT_DEFAULT, SDL_WINDOW_SHOWN);
}

// Function to render text on the screen
void UI::renderText(const std::string &text)

{

    // set black background
    SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, 255); // Black background

    // create text surface
    SDL_Surface *textSurface = TTF_RenderText_Solid(this->font, text.c_str(), this->color);
    if (!textSurface)
    {
        std::cout << "Failed to create text surface! SDL_ttf Error: " << TTF_GetError() << std::endl;
        exit(1);
    }

    this->texture = SDL_CreateTextureFromSurface(this->renderer, textSurface);
    SDL_Rect textRect;
    textRect.w = textSurface->w;
    textRect.h = textSurface->h;
    textRect.x = (SCREEN_WIDTH_DEFAULT - textRect.w) / 2;
    textRect.y = (SCREEN_HEIGHT_DEFAULT - textRect.h) / 2;
    // clear, render, and present
    SDL_RenderClear(this->renderer);
    SDL_RenderCopy(this->renderer, this->texture, NULL, &textRect);
    SDL_RenderPresent(this->renderer);
    SDL_FreeSurface(textSurface); // Free surface after creating texture
}

UI::UI()
{
    this->window = sdl_init();
    this->renderer = SDL_CreateRenderer(this->window, -1, SDL_RENDERER_ACCELERATED);
    this->font = TTF_OpenFont("/System/Library/Fonts/Supplemental/Arial.ttf", 28);
    if (!this->font)
    {
        std::cerr << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << std::endl;
        exit(1);
    }
    this->color = {255, 255, 255, 255};
    this->running = true;
}

void UI::quit()
{
    this->running = false;
    if (this->texture)
        SDL_DestroyTexture(this->texture);
    if (this->font)
        TTF_CloseFont(this->font);
    if (this->renderer)
        SDL_DestroyRenderer(this->renderer);
    if (this->window)
        SDL_DestroyWindow(this->window);
    SDL_Quit();
    TTF_Quit();
}