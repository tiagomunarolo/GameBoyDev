#include "ui.hpp"
#include "memory.hpp"
#include "ppu.hpp"
#include <unistd.h> // For sleep() and usleep()

using namespace std;

static unsigned long WHITE = 0xFFFFFFFF;
static unsigned long LIGHT_GRAY = 0xFFAAAAAA;
static unsigned long DARK_GRAY = 0xFF555555;
static unsigned long BLACK = 0xFF000000;
const int scale = 4;

static unsigned long tile_colors[4] = {WHITE, LIGHT_GRAY, DARK_GRAY, BLACK};

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
    int columns = num_cols * 8 * scale;
    int rows = num_rows * 8 * scale;

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
    columns = num_cols * 8 * scale;
    rows = num_rows * 8 * scale;

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
    SDL_SetWindowPosition(this->debug_window, x + this->w + 10, y); // move debug window to the right
#endif
}

unsigned long getColorPalletId(u8 pallete, u8 color_id)
{
    return tile_colors[(pallete >> (color_id * 2)) & 0x03];
}

void update_pixels(SDL_Surface *surface, u8 high_byte, u8 low_byte, int x, int y, u8 pallete)
{
    SDL_Rect rc;
    rc.y = y;
    rc.w = scale;
    rc.h = scale;
    for (int bit = 0; bit <= 7; bit++)
    {
        bool hi = !!(high_byte & (1 << bit));
        bool lo = !!(low_byte & (1 << bit));
        u8 color_id = (hi << 1 | lo);
        rc.x = x + (7 - bit) * scale; // bit is left bit, need to keep image not rotated
        SDL_FillRect(surface, &rc, getColorPalletId(pallete, color_id));
    }
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
        yDraw = (tile / 16) * 8 * scale;

        xDraw = (tile % 16) * 8 * scale;

        for (int tile_row = 0; tile_row < 16; tile_row += 2)

        {
            u8 low_byte = memory->vram[tile_row + 16 * tile];
            u8 high_byte = memory->vram[tile_row + 1 + 16 * tile];
            update_pixels(this->debug_screen, high_byte, low_byte, xDraw, (yDraw + tile_row / 2) * scale, pallete);
        }
    }

    render_surface(this->debug_texture, this->debug_renderer, this->debug_screen);
}
#endif

u8 reverse_bit_x(u8 value)
{
    // bit 0 -> 7, 1 ->6....
    u8 update = 0;
    for (u8 b = 0; b < 8; b++)
    {
        u8 bit = (value & 1 << b) >> b;
        update |= bit << (7 - b);
    }
    return update;
}

void UI::renderObjs()
{
    if (!ppu->IsObjEnable())
        return;

    // objects in lower address has higher priority
    int obj_counter = 0;
    for (int i = 0; i < 40; i++)
    {

        Sprite current = ppu->sprites[i];
        // Priority: 0 = No, 1 = BG and Window colors 1–3 are drawn over this OBJ
        int y_pos = current.y_pos;
        int x_pos = current.x_pos;

        int y_lower = y_pos - 16;
        int y_higher = y_lower + 8;
        int x_lower = x_pos - 8;
        int x_higher = x_lower + 8;
        int ly = ppu->GetLy();

        // ignore sprites outside screen for now
        if (ly < y_lower || ly > y_higher)
            continue;

        // ignore sprites outside screen for now
        if (y_lower < 0 || y_higher >= SCREEN_HEIGHT_DEFAULT)
            continue;

        // ignore sprites outside screen for now
        if (x_lower < 0 || x_higher >= SCREEN_WIDTH_DEFAULT)
            continue;

        obj_counter++;
        bool y_fliped = (current.attributes >> 6) & 0x1;
        bool x_fliped = (current.attributes >> 5) & 0x1;
        bool obj_pallete = (current.attributes >> 4) & 0x01;
        u8 pallete = ppu->getObjPallete(obj_pallete);
        u8 tile_pos = 0, pos = 0;
        pos = tile_pos = (ly % 8);
        if (y_fliped)
            tile_pos = 7 - tile_pos;

        u8 low_byte = memory->vram[current.tile_id * 16 + 2 * tile_pos];
        u8 high_byte = memory->vram[current.tile_id * 16 + 2 * tile_pos + 1];
        if (x_fliped)
        {
            low_byte = reverse_bit_x(low_byte);
            high_byte = reverse_bit_x(high_byte);
        }
        update_pixels(this->screen, high_byte, low_byte, x_lower * scale, (y_lower + pos) * scale, pallete);
    }
}

void UI::renderWindow()
{
    if (!ppu->IsWindowFrameEnabled())
        return;

    int wx = ppu->getWX() - 7; // WX is offset by 7 pixels
    int wy = ppu->getWY();
    int y = ppu->GetLy();

    // Window only starts rendering at WY, and WX must be visible
    if (y < wy || wx >= SCREEN_WIDTH_DEFAULT || wx < 0)
        return;

    u16 window_map_addr = ppu->getWindowTileMap();  // Window tile map base address
    u8 pallete = ppu->getBackgroundWindowPallete(); // Palette
    u16 tile_addr = ppu->getTileArea();             // Tile data base address

    int row = (y - wy / 8) % 32; // Current tile row in the window
    int start_tile = wx / 8;     // Current tile column in the window

    for (int tile_x = start_tile, x = start_tile * 8 * scale; tile_x < (SCREEN_WIDTH_DEFAULT / 8); tile_x++, x += 8 * scale)
    {
        u16 tile_index = window_map_addr + (row * 32) + tile_x;          // Correct tile index
        u8 tile_reference = memory->vram[tile_index - 0x8000];           // Tile reference from tile map
        u16 tile_data_addr = tile_addr + (tile_reference * 16) - 0x8000; // Tile data address

        int tile_row = 2 * (y % 8); // Line inside the tile (2 bytes per row)
        u8 high_byte = memory->vram[tile_data_addr + tile_row + 1];
        u8 low_byte = memory->vram[tile_data_addr + tile_row];
        update_pixels(this->screen, high_byte, low_byte, x, y, pallete);
    }
}

void UI::renderBackGround()
{
    u16 tile_map_addr = ppu->getBgTileMapAddress(); // bg tile map is 32x32, VRAM
    u16 tile_addr = ppu->getTileArea();             // tile area on vram
    int scx = ppu->getSCX();                        // SCX register
    int scy = ppu->getSCY();                        // SCY register
    u8 pallete = ppu->getBackgroundWindowPallete(); // Background pallete
    int y = ppu->GetLy();
    int row = ((scy + y) / 8) % 32; // vertical shift

    for (int tile_x = 0, x = 0; tile_x < (SCREEN_WIDTH_DEFAULT / 8); tile_x++, x = x + 8 * scale)
    {

        int col = ((scx + tile_x * 8) / 8) % 32;                         // horizontal shift
        int tile_map_index = tile_map_addr + row * 32 + col;             // tile position on 32x32 from start address
        u8 tile_reference = memory->vram[tile_map_index - 0x8000];       // tile map reference
        u16 tile_data_addr = (tile_addr + tile_reference * 16 - 0x8000); // tile data address
        int tile_row = 2 * (y % 8);
        u8 high_byte = memory->vram[tile_data_addr + tile_row + 1];
        u8 low_byte = memory->vram[tile_data_addr + tile_row];
        update_pixels(this->screen, high_byte, low_byte, x, y * scale, pallete);
    }
}

void UI::update()
{

    this->renderBackGround();
    this->renderWindow();
    this->renderObjs();
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
