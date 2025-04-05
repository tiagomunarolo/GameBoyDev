#include "ui.hpp"
#include "ppu.hpp"

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

static unsigned long getColor(u8 high, u8 low, u8 pallete, int x)
{
    x = x % 8;
    u8 color_id = (((high >> (7 - x)) & 0x01) << 1) | (((low >> (7 - x)) & 0x01));
    return COLORS[((pallete >> (color_id * 2))) & 0x03];
}

Pixels PixelProcessingUnit::getBackgroundPixels(int y)
{
    u16 tile_area = this->getTileArea();
    int row = ((this->getSCY() + y) / 8) % 32;                         // vertical shift
    int col = ((this->getSCX() + this->pixel_x) / 8) % 32;             // horizontal shift
    int tile_map_index = this->getBgTileMapAddress() + row * 32 + col; // tile position on 32x32 from start address
    u8 tile_reference = memory->vram[tile_map_index - 0x8000];         // tile map reference
    u16 tile_data_addr = 0x00;
    if (tile_area == 0x8000)
        tile_data_addr = tile_area + tile_reference * 16 - 0x8000; // tile data address
    else
        tile_data_addr = 0x9000 + (int8_t)tile_reference * 16 - 0x8000;

    int tile_row = 2 * (y % 8);
    u8 high_byte = 0x00;
    u8 low_byte = 0x00;
    if (this->BgWindowEnablePriority())
    {
        // if bg window priority is on
        // When Bit 0 is cleared, both background and window become blank (white)
        high_byte = memory->vram[tile_data_addr + tile_row + 1];
        low_byte = memory->vram[tile_data_addr + tile_row];
    }
    Pixels pixels = {this->pixel_x, y, 0, false};
    pixels.color = getColor(high_byte, low_byte, getBackgroundWindowPallete(), this->pixel_x);
    return pixels;
}

Pixels PixelProcessingUnit::getWindowPixels(int y)
{
    int wx = this->getWX() - 7; // WX is offset by 7 pixels
    int wy = this->getWY();

    // Window only starts rendering at WY, and WX must be visible
    if (y < wy || wx >= SCREEN_WIDTH_DEFAULT || wx < 0 || this->pixel_x < wx)
        return Pixels{-1, -1, 0, false};

    u16 window_map_addr = this->getWindowTileMap(); // Window tile map base address
    u16 tile_area = this->getTileArea();            // Tile data base address

    int row = (this->internalWy / 8) % 32;     // Current tile row in the window is based on internal window counter
    int col = ((this->pixel_x - wx) / 8) % 32; // Current tile column in the window

    u16 tile_index = window_map_addr + (row * 32) + col;   // Correct tile index
    u8 tile_reference = memory->vram[tile_index - 0x8000]; // Tile reference from tile map
    u16 tile_data_addr = 0x00;                             // Tile data address

    if (tile_area == 0x8000)
        tile_data_addr = tile_area + (tile_reference * 16) - 0x8000; // tile data address
    else
        tile_data_addr = 0x9000 + (int8_t)tile_reference * 16 - 0x8000;

    int tile_row = 2 * (this->internalWy % 8); // Line inside the tile (2 bytes per row)
    u8 high_byte = memory->vram[tile_data_addr + tile_row + 1];
    u8 low_byte = memory->vram[tile_data_addr + tile_row];
    Pixels pixels = {this->pixel_x, y, 0, false};
    pixels.color = getColor(high_byte, low_byte, getBackgroundWindowPallete(), this->pixel_x);
    return pixels;
}

Pixels PixelProcessingUnit::getObjectsPixels(int y)
{

    int objSize = this->getObjectSize();

    std::vector<Sprite> valid_objs = {};
    for (int i = 0; i < 40; i++)
    {
        if (valid_objs.size() >= 10)
            break;

        Sprite current = this->sprites[i];
        int y_pos = current.y_pos;
        int y_lower = y_pos - 16;
        int y_higher = y_lower + objSize - 1;

        // ignore sprites outside screen for now
        if (y < y_lower || y > y_higher)
            continue;

        // ignore sprites outside screen for now
        if (y_lower < 0 || y_higher >= SCREEN_HEIGHT_DEFAULT)
            continue;

        valid_objs.push_back(current);
    }

    // sort by x position
    std::sort(valid_objs.begin(), valid_objs.end(), [](Sprite a, Sprite b)
              { return a.x_pos < b.x_pos; });
    // objects in lower address has higher priority
    for (auto sprite : valid_objs)
    {

        // priority bit
        // 0 = obj is drawn over bg and window
        // 1 = obj is drawn below bg

        // off screen sprite
        if (sprite.x_pos == 0 || sprite.x_pos >= 168)
            continue;

        if (sprite.x_pos - 8 > this->pixel_x || this->pixel_x > sprite.x_pos - 1)
            continue;

        // Priority: 0 = No, 1 = BG and Window colors 1–3 are drawn over this OBJ
        bool y_fliped = (sprite.attributes >> 6) & 0x1;
        bool x_fliped = (sprite.attributes >> 5) & 0x1;
        u8 pallete = this->getObjPallete((sprite.attributes >> 4) & 0x01);

        int tile_pos = 0, pos = 0;
        pos = tile_pos = (y % objSize);
        int tile_id = sprite.tile_id * 16;
        if (objSize == 16)
            tile_id = (sprite.tile_id & 0xFE) * 16;
        if (objSize == 16 && pos >= 8)
        {
            tile_id = (sprite.tile_id | 0x1) * 16;
            pos = tile_pos = (y % 8);
        }
        if (y_fliped)
            tile_pos = 7 - tile_pos;

        u8 low_byte = memory->vram[tile_id + 2 * tile_pos];
        u8 high_byte = memory->vram[tile_id + 2 * tile_pos + 1];
        if (x_fliped)
        {
            low_byte = reverse_bit_x(low_byte);
            high_byte = reverse_bit_x(high_byte);
        }
        Pixels pixels = {this->pixel_x, y, 0, false};
        pixels.bg_window_priority = (sprite.attributes & 0x80) != 0;
        pixels.color = getColor(high_byte, low_byte, pallete, this->pixel_x);
        return pixels;
    }
    return Pixels{-1, -1, 0, false};
}
void PixelProcessingUnit::setPixels()
{
    int y = this->getLy();
    Pixels pixels = this->getBackgroundPixels(y);
    if (IsWindowFrameEnabled() && BgWindowEnablePriority())
    {
        Pixels windowPixels = this->getWindowPixels(y);
        // if valid window layer pixel
        if (windowPixels.x != -1 && windowPixels.y != -1)
            pixels.color = windowPixels.color;
    }

    if (IsObjEnable())
    {
        Pixels objPixels = this->getObjectsPixels(y);
        if (objPixels.x != -1 && objPixels.y != -1)
        { // if valid object

            if (objPixels.bg_window_priority)
            {
                if (pixels.color == WHITE)
                    pixels.color = objPixels.color;
            }
            else
            {
                if (objPixels.color != WHITE)
                    pixels.color = objPixels.color;
            }
        }
    }

    this->frame[this->pixel_x][y] = pixels;
    this->pixel_x = this->pixel_x + 1;
}

Pixels (*PixelProcessingUnit::getFrame())[SCREEN_WIDTH_DEFAULT]
{
    return this->frame;
}
