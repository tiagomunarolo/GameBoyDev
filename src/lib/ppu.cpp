#include "ppu.hpp"
#include "ui.hpp"
#include "interruption.hpp"
#include <thread>
#include <chrono>

using namespace std;

static const int MAX_SCANLINES = 154;
static const int DOTS_PER_LINE = 456;
static int vblank_lines = 0;

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

Sprite::Sprite(u8 y_pos, u8 x_pos, u8 tile_id, u8 attributes)
{
    this->y_pos = y_pos;
    this->x_pos = x_pos;
    this->tile_id = tile_id;
    this->attributes = attributes;
}

PixelProcessingUnit::PixelProcessingUnit(Memory *mem)
{
    // for (int i = 0; i < 40; i++)
    //     this->sprites[i] = Sprite(0, 0, 0, 0); // initialize each sprite to 0
    this->mem = mem;
    this->mode = VBlankMode;
    // num cycles of cpu timer to keep sync
    this->cycles = 0;
    // set current ticks
    this->current_dot = 0;
    // current pixel x position
    this->pixel_x = 0;
    this->internalWy = 0;
    // LCD
    mem->io[0x40] = 0x91;
    mem->io[0x41] = 0x81;
    mem->io[0x42] = 0x00;
    mem->io[0x43] = 0x00;
    mem->io[0x44] = 0x90;
    mem->io[0x45] = 0x00;
    mem->io[0x46] = 0xff;
    mem->io[0x47] = 0xfc;
    mem->io[0x48] = 0xff;
    mem->io[0x49] = 0xff;
    mem->io[0x4A] = 0x00;
    mem->io[0x4B] = 0x00;
    lcdc = &mem->io[0x40];
    stat = &mem->io[0x41];
    scy = &mem->io[0x42];
    scx = &mem->io[0x43];
    ly = &mem->io[0x44];
    lyc = &mem->io[0x45];
    bgp = &mem->io[0x47];
    obp0 = &mem->io[0x48];
    obp01 = &mem->io[0x49];
    wy = &mem->io[0x4A];
    wx = &mem->io[0x4B];
}

PpuMode PixelProcessingUnit::GetPpuMode()
{
    // lower two bits
    u8 mode = *this->stat & 0b11;
    switch (mode)
    {
    case 0x00:
        return HBlankMode;
    case 0x01:
        return VBlankMode;
    case 0x02:
        return OAMScanMode;
    case 0x03:
        return RenderingMode;
    default:
        throw std::runtime_error("Invalid PPU mode");
    }
}

bool PixelProcessingUnit::IsLcdOn()
{
    // BIT 7 indicates if lcd is on on lcdc register
    return *this->lcdc & 0x80;
}

u16 PixelProcessingUnit::getWindowTileMap()
{
    // BIT 6 - window tile map area
    if (*this->lcdc & 0x40)
        // == 1
        return 0x9C00;

    // == 0
    return 0x9800;
}

bool PixelProcessingUnit::IsWindowFrameEnabled()
{
    // BIT 5
    return (*this->lcdc & 0x20);
}

u16 PixelProcessingUnit::getTileArea()
{
    // BIT 4
    if (*this->lcdc & 0x10)
        // == 1
        return 0x8000;

    // == 0
    return 0x8800;
}

u16 PixelProcessingUnit::getBgTileMapAddress()
{
    // BIT 3
    if (*this->lcdc & 0x08)
        // == 1
        return 0x9C00;

    // == 0
    return 0x9800;
}

u8 PixelProcessingUnit::getObjectSize()
{
    // BIT 2
    if (*this->lcdc & 0x04)
        // == 1
        return 16;

    // == 0
    return 8;
}

void PixelProcessingUnit::updateLy()
{
    *this->ly += 1;
    this->pixel_x = 0;
    if (this->IsWindowFrameEnabled() &&
        this->getLy() > this->getWY() && this->getWY() >= 0 &&
        this->getWY() <= 143 && this->getWX() >= 7 && this->getWX() <= 166)
    {
        this->internalWy = internalWy + 1;
    }

    if (*this->ly == *this->lyc)
        *this->stat |= 0b00000010;
    else
        *this->stat &= ~0b00000010;

    // bit 6 LYC int select
    // set interruption flag register FF0F
    if (*this->stat & 0x40 && *this->ly == *this->lyc)
        interruption->setInterruption(LCD);
}

u8 PixelProcessingUnit::getLy()
{
    return *this->ly;
}

bool PixelProcessingUnit::IsObjEnable()
{
    // BIT 1
    return *this->lcdc & 0b10;
}

bool PixelProcessingUnit::BgWindowEnablePriority()
{
    // BIT 0
    return *this->lcdc & 0b01;
}

u8 PixelProcessingUnit::getSCX()
{
    return *this->scx;
}

u8 PixelProcessingUnit::getSCY()
{
    return *this->scy;
}

u8 PixelProcessingUnit::getWX()
{
    return *this->wx;
}

u8 PixelProcessingUnit::getWY()
{
    return *this->wy;
}

bool PixelProcessingUnit::isRendering()
{
    return this->mode == RenderingMode;
}

void PixelProcessingUnit::finishRendering()
{
    this->mode = HBlankMode;
}

void PixelProcessingUnit::setCycles(u8 value)
{

    this->cycles += value;
}

u8 PixelProcessingUnit::getBackgroundWindowPallete()
{
    return *this->bgp;
}

u8 PixelProcessingUnit::getObjPallete(bool palletBit)
{
    return palletBit ? *this->obp01 : *this->obp0;
}

void PixelProcessingUnit::runOamMode()
{
    this->mode = OAMScanMode;
    *this->stat = ((*this->stat & 0xFC) | 0x2);
    if (this->cycles < 80)
        return;
    // FE00	FE9F	Object attribute memory (OAM)
    for (int i = 0; i <= 40; i++)
    {
        this->sprites[i].y_pos = mem->oam[4 * i];
        this->sprites[i].x_pos = mem->oam[4 * i + 1];
        this->sprites[i].tile_id = mem->oam[4 * i + 2];
        this->sprites[i].attributes = mem->oam[4 * i + 3];
    }

    this->cycles -= 80;
    this->current_dot += 80;
    this->mode = RenderingMode;
}

void PixelProcessingUnit::runRenderMode()
{
    *this->stat = ((*this->stat & 0xFC) | 0x3);
    while (this->mode == RenderingMode)
        ;
    this->current_dot += 172; // minimum duration of mode 3
    this->mode = HBlankMode;
    this->current_dot += cycles;
    this->cycles = 0;
}

void PixelProcessingUnit::runVblankMode()
{
    if (this->cycles < DOTS_PER_LINE)
        return;

    this->cycles -= DOTS_PER_LINE;
    vblank_lines += 1;

    if (vblank_lines == 10 || vblank_lines == MAX_SCANLINES)
    {
        this->mode = OAMScanMode;
        vblank_lines = 0;
        *this->ly = 0;
        this->current_dot = 0;
    }

    if (*this->ly == 144)
        interruption->setInterruption(VBlank);
    *this->stat = ((*this->stat & 0xFC) | 0x1);
    this->internalWy = 0; // internal window line counter is reset on Vblank
    this->updateLy();
}

void PixelProcessingUnit::runHblankMode()
{
    *this->stat = (*this->stat & 0xFC);
    this->pixel_x = 0; // set pixel x to 0
    this->current_dot += this->cycles;
    int diff = current_dot - cycles;

    if (this->current_dot < DOTS_PER_LINE)
    {
        this->cycles -= diff;
        return;
    }
    if (*ly == SCREEN_HEIGHT_DEFAULT - 1)
        this->mode = VBlankMode;
    else
    {
        this->current_dot = 0;
        this->mode = OAMScanMode;
        this->updateLy();
    }
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

std::vector<Pixels> PixelProcessingUnit::getPixels(int y)
{
    std::vector<Pixels> pixelsVec;
    while (pixelsVec.size() < SCREEN_WIDTH_DEFAULT)
    {
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

        this->pixel_x = (this->pixel_x + 1) % SCREEN_WIDTH_DEFAULT;
        pixelsVec.push_back(pixels);
    }
    return pixelsVec;
}

// 1 ppu dot is 4 cpu cycles
void PixelProcessingUnit::run()
{
    if (mode == VBlankMode)
        this->runVblankMode();
    else if (mode == OAMScanMode)
        this->runOamMode();
    else if (mode == RenderingMode)
        this->runRenderMode();
    else
        this->runHblankMode();
}

PixelProcessingUnit *ppu = nullptr;