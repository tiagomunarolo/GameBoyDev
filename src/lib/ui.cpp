#include "ui.hpp"
#include "ppu.hpp"

using namespace std;

// intialize SDL and SDL_ttf. Returns SDL_Window
void UI::sdl_init()
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
    // main window
    int num_cols = 20;
    int num_rows = 18;
    int columns = num_cols * 8 * SCALE;
    int rows = num_rows * 8 * SCALE;

    SDL_CreateWindowAndRenderer(columns, rows, 0,
                                &this->window, &this->renderer);
    // main surface
    this->screen = SDL_CreateRGBSurface(
        0,          // flags
        columns,    // width
        rows,       // height
        32,         // depth
        0xFF000000, // red mask
        0x00FF0000, // green mask
        0x0000FF00, // blue mask
        0);         // alpha mask

    // main texture
    this->texture = SDL_CreateTexture(this->renderer,
                                      SDL_PIXELFORMAT_ARGB8888,
                                      SDL_TEXTUREACCESS_STREAMING,
                                      columns,
                                      rows);
    SDL_FillRect(this->screen, NULL, WHITE);

#ifdef DEBUG_UI
    // debug window
    num_cols = 16;
    num_rows = 24;
    columns = num_cols * 8 * SCALE;
    rows = num_rows * 8 * SCALE;

    SDL_CreateWindowAndRenderer(columns, rows, 0,
                                &this->debug_window, &this->debug_renderer);

    // debug surface
    this->debug_screen = SDL_CreateRGBSurface(
        0,           // flags
        columns,     // width
        rows,        // height
        32,          // depth
        0x00FF0000,  // red mask
        0x0000FF00,  // green mask
        0x000000FF,  // blue mask
        0xFF000000); // alpha mask

    // debug texture
    this->debug_texture = SDL_CreateTexture(this->debug_renderer,
                                            SDL_PIXELFORMAT_ARGB8888,
                                            SDL_TEXTUREACCESS_STREAMING,
                                            columns,
                                            rows);
#endif

    int x, y;
    SDL_GetWindowPosition(this->window, &x, &y); // get main screen position
#ifdef DEBUG_UI
    SDL_SetWindowPosition(this->debug_window, x + SCREEN_WIDTH_DEFAULT + 10, y); // move debug window to the right
#endif
}

unsigned long getColorPalletId(u8 pallete, u8 color_id)
{
    return tile_colors[(pallete >> (color_id * 2)) & 0x03];
}

SDL_Rect updatePixelsView(SDL_Surface *surface, Pixels p)
{
    SDL_Rect rc;
    rc.y = p.y * SCALE;
    rc.w = SCALE;
    rc.h = SCALE;
    for (int bit = 0; bit <= 7; bit++)
    {
        rc.x = p.x * SCALE + (7 - bit) * SCALE; // bit is left bit, need to keep image not rotated
        SDL_FillRect(surface, &rc, getColorPalletId(p.palleteColors[7 - bit], p.colors[7 - bit]));
    }
    return rc;
}

void render_surface(SDL_Texture *texture, SDL_Renderer *renderer, SDL_Surface *surface)
{
    SDL_UpdateTexture(texture, NULL, surface->pixels, surface->pitch);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

#ifdef DEBUG_UI
void UI::update_dbg_window()
{
    u8 pallete = memory->io[0x47];
    int xDraw = 0;
    int yDraw = 0;
    // 384 tiles, 16x24
    for (int tile = 0; tile < 384; tile++)
    {
        // every 16 tiles, move to next row
        yDraw = (tile / 16) * 8 * SCALE;

        xDraw = (tile % 16) * 8 * SCALE;

        for (int tile_row = 0; tile_row < 16; tile_row += 2)

        {
            u8 low_byte = memory->vram[tile_row + 16 * tile];
            u8 high_byte = memory->vram[tile_row + 1 + 16 * tile];
            updatePixelsView(this->debug_screen, high_byte, low_byte, xDraw, (yDraw + tile_row / 2) * SCALE, pallete);
        }
    }

    render_surface(this->debug_texture, this->debug_renderer, this->debug_screen);
}
#endif

void UI::update(bool render)
{
    Pixels pixels = ppu->getPixels();
    updatePixelsView(this->screen, pixels);
    if (render)
        render_surface(this->texture, this->renderer, this->screen);
#ifdef DEBUG_UI
    this->update_dbg_window();
#endif
};

UI::UI()
{
    this->sdl_init();
    this->running = true;
}

void UI::check_event()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
        if (event.type == SDL_QUIT || (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE))
            this->running = false;
}

void UI::quit()
{
    this->running = false;
    // destroy textures
    if (this->texture)
        SDL_DestroyTexture(this->texture);
    // destroy rendereres
    if (this->renderer)
        SDL_DestroyRenderer(this->renderer);
    // destroy windows
    if (this->window)
        SDL_DestroyWindow(this->window);
#ifdef DEBUG_UI
    if (this->debug_texture)
        SDL_DestroyTexture(this->debug_texture);
    if (this->debug_renderer)
        SDL_DestroyRenderer(this->debug_renderer);
    if (this->debug_window)
        SDL_DestroyWindow(this->debug_window);
#endif

    SDL_Quit();
    TTF_Quit();
}

UI *ui = nullptr;
