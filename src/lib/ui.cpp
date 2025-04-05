#include "ui.hpp"
#include "ppu.hpp"
#include "joypad.hpp"
#include "dma.hpp"
#include "interruption.hpp"

#include <chrono>
#include <iostream>
#include <thread>

using namespace std;

Uint32 start_time = SDL_GetTicks(); // Get start time
int frame_count = 0;
int FPS_DESIRED = 60;

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
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

// intialize SDL and SDL_ttf. Returns SDL_Window
void UI::sdl_init()
{
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
        yDraw = (tile / 16) * 8;
        xDraw = (tile % 16) * 8;

        for (int tile_row = 0; tile_row < 16; tile_row += 2)

        {
            u8 low_byte = memory->vram[tile_row + 16 * tile];
            u8 high_byte = memory->vram[tile_row + 1 + 16 * tile];
            for (int i = 0; i < 8; i++)
            {
                int x = (xDraw + i) % 8;
                u8 color_id = (((high_byte >> (7 - x)) & 0x01) << 1) | (((low_byte >> (7 - x)) & 0x01));
                Pixels pixel = {xDraw + x, yDraw + tile_row / 2, COLORS[((pallete >> (color_id * 2))) & 0x03]};
                updatePixelsView(debug_screen, pixel);
            }
        }
    }
    render_surface(debug_texture, debug_renderer, debug_screen);
}
#endif

void UI::update()
{
    if (ppu->getLy() != SCREEN_HEIGHT_DEFAULT)
        return;

    Uint32 current_time = SDL_GetTicks();
    // get all pixels from current scanline
    Pixels(*frame)[SCREEN_WIDTH_DEFAULT] = ppu->getFrame();
    for (int row = 0; row < SCREEN_HEIGHT_DEFAULT; row++)
        for (int col = 0; col < SCREEN_WIDTH_DEFAULT; col++)
            updatePixelsView(screen, frame[row][col]);

    // render elements
    render_surface(texture, renderer, screen);
    frame_count++;

    // Print FPS every second
    if (current_time - start_time >= 1000)
    {
        std::cout << "FPS: " << frame_count << std::endl;
        frame_count = 0;
        start_time = current_time;
    }

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
    {
        if (event.type == SDL_QUIT || (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE))
            this->running = false;

        if (event.type != SDL_KEYUP && event.type != SDL_KEYDOWN)
            return;

        bool press = !(event.type == SDL_KEYUP);

        SDL_Keycode key = event.key.keysym.sym;

        if (key == SDLK_DOWN)
        {
            joypad->pressButton(DownBtn, press);
        }
        else if (key == SDLK_UP)
        {
            joypad->pressButton(UpBtn, press);
        }
        else if (key == SDLK_LEFT)
        {
            joypad->pressButton(LeftBtn, press);
        }
        else if (key == SDLK_RIGHT)
        {
            joypad->pressButton(RightBtn, press);
        }
        else if (key == SDLK_a)
        {
            joypad->pressButton(ABtn, press);
        }
        else if (key == SDLK_s)
        {
            joypad->pressButton(BBtn, press);
        }
        else if (key == SDLK_p)
        {
            joypad->pressButton(StartBtn, press);
        }
        else if (key == SDLK_SPACE)
        {
            joypad->pressButton(SelectBtn, press);
        }
    }
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
}

UI *ui = nullptr;
