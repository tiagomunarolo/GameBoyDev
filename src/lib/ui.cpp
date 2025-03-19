#include "ui.hpp"
#include "bus.hpp"
#include <unistd.h> // For sleep() and usleep()

using namespace std;

static unsigned long tile_colors[4] = {0xFFFFFFFF, 0xFFAAAAAA, 0xFF555555, 0xFF000000};
const int scale = 4;

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
    int columns = num_cols * 8 * scale + (num_cols * scale);
    int rows = num_rows * 8 * scale + (num_rows * scale);

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
    SDL_FillRect(this->screen, NULL, tile_colors[0]);

#ifdef DEBUG
    // debug window
    num_cols = 16;
    num_rows = 24;
    columns = num_cols * 8 * scale;
    rows = num_rows * 8 * scale;

    SDL_CreateWindowAndRenderer(columns, rows, 0,
                                &this->debug_window, &this->debug_renderer);

    // debug surface
    this->debug_screen = SDL_CreateRGBSurface(
        0,                            // flags
        columns + (num_cols * scale), // width
        rows + (num_rows * scale),    // height
        32,                           // depth
        0x00FF0000,                   // red mask
        0x0000FF00,                   // green mask
        0x000000FF,                   // blue mask
        0xFF000000);                  // alpha mask

    // debug texture
    this->debug_texture = SDL_CreateTexture(this->debug_renderer,
                                            SDL_PIXELFORMAT_ARGB8888,
                                            SDL_TEXTUREACCESS_STREAMING,
                                            columns + (num_cols * scale),
                                            rows + (num_rows * scale));
#endif

    int x, y;
    SDL_GetWindowPosition(this->window, &x, &y);                    // get main screen position
    SDL_SetWindowPosition(this->debug_window, x + this->w + 10, y); // move debug window to the right
}

void update_pixels(SDL_Surface *surface, u8 high_byte, u8 low_byte, int x, int y, u8 tile_y)
{
    SDL_Rect rc;

    for (int bit = 0; bit <= 7; bit++)
    {
        u8 hi = (high_byte & (1 << bit)) == 0 ? 0 : 1;
        u8 lo = (low_byte & (1 << bit)) == 0 ? 0 : 1;
        u8 color = (hi << 1 | lo);
        // each pixel has n*pixel due to scale applied
        // (x,y) represents upper left corner position
        // x is current tile
        // bit represents current pixel bit of 8x8 (horizontal shift)
        // y depends on current tile position
        rc.x = x + (7 - bit) * scale; // bit is left bit, need to keep image not rotated
        rc.y = y + (tile_y / 2) * scale;
        rc.w = scale;
        rc.h = scale;
        SDL_FillRect(surface, &rc, tile_colors[color]);
    }
}

void display_tile(SDL_Surface *surface, u16 current_tile, int x, int y, Memory *mem)
{

    SDL_Rect rc;

    // tile is 8x8 => uses 16 bytes
    for (int tile_y = 0; tile_y < 16; tile_y += 2)
    {

        u8 low_byte = mem->vram[current_tile * 16 + tile_y];
        u8 high_byte = mem->vram[current_tile * 16 + tile_y + 1];
        update_pixels(surface, high_byte, low_byte, x, y, tile_y);
    }
}

void render_surface(SDL_Texture *texture, SDL_Renderer *renderer, SDL_Surface *surface)
{
    SDL_UpdateTexture(texture, NULL, surface->pixels, surface->pitch);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

#ifdef DEBUG
void UI::update_dbg_window()
{
    int xDraw = 0;
    int yDraw = 0;
    int tileNum = 0;

    // 384 tiles, 16x24
    for (int y = 0; y < 24; y++)
    {

        for (int x = 0; x < 16; x++)

        {

            display_tile(this->debug_screen, tileNum, xDraw + (x * scale), yDraw + (y * scale), this->mem);
            xDraw += (8 * scale);
            tileNum++;
        }

        yDraw += (8 * scale); // every tile has 8 pixels
        xDraw = 0;            // return to left side of window rendered
    }

    render_surface(this->debug_texture, this->debug_renderer, this->debug_screen);
}
#endif

void UI::update_window()
{

    // bg tile map is 32x32
    BgAddress bg = this->ppu->get_bg_tile_map();
    TileMap tileMap = this->ppu->get_tile_range();
    int scx = this->ppu->get_scx();
    int scy = this->ppu->get_scy();
    int xDraw = 0;
    int yDraw = 0;
    for (int y = 0; y < 18; y++)
    {

        for (int x = 0; x < 20; x++)
        {
            u8 row = (scy / 8 + y) % 32;                              // vertical shift
            u8 col = ((scx / 8) + x) % 32;                            // horizontal shift
            int tile_index = bg.start + row * 32 + col;               // tile position on 32x32 from start address
            u8 tile_reference = this->mem->vram[tile_index - 0x8000]; // data inside current tile map
            display_tile(this->screen, tile_reference, xDraw + (x * scale), yDraw + (y * scale), this->mem);
            xDraw += (8 * scale);
        }
        yDraw += (8 * scale); // every tile has 8 pixels
        xDraw = 0;            // return to left side of window rendered
    }

    render_surface(this->texture, this->renderer, this->screen);
}

void UI::update()
{
    if (this->ppu->lcd_is_on())
        this->update_window();
#ifdef DEBUG
    this->update_dbg_window();
#endif
};

UI::UI(Memory *mem, PixelProcessingUnit *ppu)
{

    this->mem = mem;
    this->ppu = ppu;
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
    // destroy font
    if (this->font)
        TTF_CloseFont(this->font);
    // destroy textures
    if (this->texture)
        SDL_DestroyTexture(this->texture);
    if (this->debug_texture)
        SDL_DestroyTexture(this->debug_texture);
    // destroy rendereres
    if (this->renderer)
        SDL_DestroyRenderer(this->renderer);
    if (this->debug_renderer)
        SDL_DestroyRenderer(this->debug_renderer);
    // destroy windows
    if (this->window)
        SDL_DestroyWindow(this->window);
    if (this->debug_window)
        SDL_DestroyWindow(this->debug_window);
    SDL_Quit();
    TTF_Quit();
}