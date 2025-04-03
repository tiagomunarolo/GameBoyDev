#include "ui.hpp"
#include "ppu.hpp"

using namespace std;

static void updatePixelsView(SDL_Surface *surface, Pixels p)
{
    SDL_Rect rc;
    rc.y = p.y * SCALE;
    rc.x = p.x * SCALE;
    rc.w = SCALE;
    rc.h = SCALE;
    SDL_FillRect(surface, &rc, p.color);
}

static void render_surface(SDL_Texture *texture, SDL_Renderer *renderer, SDL_Surface *surface)
{
    SDL_UpdateTexture(texture, NULL, surface->pixels, surface->pitch);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

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

    SDL_CreateWindowAndRenderer(columns, rows, 0, &window, &renderer);
    // main surface
    screen = SDL_CreateRGBSurface(
        0,          // flags
        columns,    // width
        rows,       // height
        32,         // depth
        0xFF000000, // red mask
        0x00FF0000, // green mask
        0x0000FF00, // blue mask
        0);         // alpha mask

    // main texture
    texture = SDL_CreateTexture(renderer,
                                SDL_PIXELFORMAT_ARGB8888,
                                SDL_TEXTUREACCESS_STREAMING,
                                columns,
                                rows);
    SDL_FillRect(screen, NULL, WHITE);
    render_surface(texture, renderer, screen);

#ifdef DEBUG_UI
    // debug window
    num_cols = 16;
    num_rows = 24;
    columns = num_cols * 8 * SCALE;
    rows = num_rows * 8 * SCALE;

    SDL_CreateWindowAndRenderer(columns, rows, 0,
                                &debug_window, &debug_renderer);

    // debug surface
    debug_screen = SDL_CreateRGBSurface(
        0,           // flags
        columns,     // width
        rows,        // height
        32,          // depth
        0x00FF0000,  // red mask
        0x0000FF00,  // green mask
        0x000000FF,  // blue mask
        0xFF000000); // alpha mask

    // debug texture
    debug_texture = SDL_CreateTexture(debug_renderer,
                                      SDL_PIXELFORMAT_ARGB8888,
                                      SDL_TEXTUREACCESS_STREAMING,
                                      columns,
                                      rows);
#endif

    int x, y;
    SDL_GetWindowPosition(window, &x, &y); // get main screen position
#ifdef DEBUG_UI
    SDL_SetWindowPosition(debug_window, x + SCREEN_WIDTH_DEFAULT + 10, y); // move debug window to the right
#endif
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
            updatePixelsView(debug_screen, high_byte, low_byte, xDraw, (yDraw + tile_row / 2) * SCALE, pallete);
        }
    }

    render_surface(debug_texture, debug_renderer, debug_screen);
}
#endif

void UI::update(bool render)
{
    Pixels pixels = ppu->getPixels();
    updatePixelsView(screen, pixels);
    if (render)
        render_surface(texture, renderer, screen);
#ifdef DEBUG_UI
    update_dbg_window();
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
    if (texture)
        SDL_DestroyTexture(texture);
    // destroy rendereres
    if (renderer)
        SDL_DestroyRenderer(renderer);
    // destroy windows
    if (window)
        SDL_DestroyWindow(window);
#ifdef DEBUG_UI
    if (debug_texture)
        SDL_DestroyTexture(debug_texture);
    if (debug_renderer)
        SDL_DestroyRenderer(debug_renderer);
    if (debug_window)
        SDL_DestroyWindow(debug_window);
#endif

    SDL_Quit();
    TTF_Quit();
}

UI *ui = nullptr;
