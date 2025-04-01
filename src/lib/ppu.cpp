#include "ppu.hpp"
#include "ui.hpp"
#include "interruption.hpp"

using namespace std;

static const int MAX_SCANLINES = 154;
static const int DOTS_PER_LINE = 456;

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
    return *this->lcdc & 0x20;
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

u8 PixelProcessingUnit::GetObjSize()
{
    // BIT 2
    if (*this->lcdc & 0x04)
        // == 1
        return 16;

    // == 0
    return 8;
}

void PixelProcessingUnit::UpdateLy()
{
    *this->ly += 1;
    if (*this->ly == *this->lyc)
        *this->stat |= 0b00000010;
    else
    {
        *this->stat &= ~0b00000010;
    }

    if (*this->stat & 0x40 && *this->ly == *this->lyc)
    { // bit 6 LYC int select
        // set interruption flag register FF0F
        interruption->setInterruption(LCD);
    }
}

u8 PixelProcessingUnit::GetLy()
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

void PixelProcessingUnit::setCycles(u8 value)
{
    if (this->IsLcdOn())
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

    this->cycles -= 79;
    this->current_dot = 80;
}

void PixelProcessingUnit::runRenderMode()
{
    this->mode = RenderingMode;
    if (this->IsLcdOn() && this->GetLy() >= 0 && this->GetLy() <= 143)
        ui->update(this->GetLy() == 143);
    *this->stat = ((*this->stat & 0xFC) | 0x3);
}
void PixelProcessingUnit::runVblankMode()
{
    this->mode = VBlankMode;
    *this->stat = ((*this->stat & 0xFC) | 0x1);
}

void PixelProcessingUnit::runHblankMode()
{
    this->mode = HBlankMode;
    *this->stat = (*this->stat & 0xFC);
}

Pixels PixelProcessingUnit::getBackgroundPixels()
{
    int y = this->GetLy();
    u16 tile_area = this->getTileArea();
    int row = ((this->getSCY() + y) / 8) % 32;                         // vertical shift
    int col = ((this->getSCX() + this->pixel_x) / 8) % 32;             // horizontal shift
    int tile_map_index = this->getBgTileMapAddress() + row * 32 + col; // tile position on 32x32 from start address
    u8 tile_reference = memory->vram[tile_map_index - 0x8000];         // tile map reference
    u16 shift_addr = tile_reference * 16 - 0x8000;
    u16 tile_data_addr = 0x00;
    if (tile_area == 0x8000)
        tile_data_addr = (tile_area + shift_addr); // tile data address
    else
    {
        if (tile_reference <= 127)
            tile_data_addr = (0x9000 + shift_addr); // tile data address
        else
            tile_data_addr = (0x8800 + shift_addr); // tile data address
    }
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
    return Pixels{this->pixel_x, y, low_byte, high_byte, this->getBackgroundWindowPallete()};
}

Pixels PixelProcessingUnit::getWindowPixels()
{
    int wx = this->getWX() - 7; // WX is offset by 7 pixels
    int wy = this->getWY();
    int y = this->GetLy();

    // Window only starts rendering at WY, and WX must be visible
    if (y < wy || wx >= SCREEN_WIDTH_DEFAULT || wx < 0 || this->pixel_x < wx)
        return Pixels{-1, -1, 0, 0, 0};

    u16 window_map_addr = this->getWindowTileMap();  // Window tile map base address
    u8 pallete = this->getBackgroundWindowPallete(); // Palette
    u16 tile_area = this->getTileArea();             // Tile data base address

    int row = (y - wy / 8) % 32; // Current tile row in the window
    int col = wx / 8;            // Current tile column in the window

    u16 tile_index = window_map_addr + (row * 32) + col;   // Correct tile index
    u8 tile_reference = memory->vram[tile_index - 0x8000]; // Tile reference from tile map
    u16 shift_addr = (tile_reference * 16) - 0x8000;
    u16 tile_data_addr = 0x00; // Tile data address

    if (tile_area == 0x8000)
        tile_data_addr = (tile_area + shift_addr); // tile data address
    else
    {
        if (tile_reference <= 127)
            tile_data_addr = (0x9000 + shift_addr); // tile data address
        else
            tile_data_addr = (0x8800 + shift_addr); // tile data address
    }

    int tile_row = 2 * (y % 8); // Line inside the tile (2 bytes per row)
    u8 high_byte = memory->vram[tile_data_addr + tile_row + 1];
    u8 low_byte = memory->vram[tile_data_addr + tile_row];
    return Pixels{this->pixel_x, y, low_byte, high_byte, pallete};
}

Pixels PixelProcessingUnit::getObjectsPixels()
{

    int ly = this->GetLy();
    int objSize = this->GetObjSize();

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
        if (ly < y_lower || ly > y_higher)
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
        if (sprite.attributes & 0x80)
            continue;

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
        pos = tile_pos = (ly % objSize);
        int tile_id = sprite.tile_id * 16;
        if (objSize == 16)
            tile_id = (sprite.tile_id & 0xFE) * 16;
        if (objSize == 16 && pos >= 8)
        {
            tile_id = (sprite.tile_id + 1) * 16;
            pos = tile_pos = (ly % 8);
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
        return Pixels{this->pixel_x, ly, low_byte, high_byte, pallete};
    }
    return Pixels{-1, -1, 0, 0, 0};
}

Pixels PixelProcessingUnit::getPixels()
{
    Pixels pixels;
    pixels = this->getBackgroundPixels();
    Pixels objPixels = Pixels{-1, -1, 0, 0, 0};
    Pixels windowPixels = Pixels{-1, -1, 0, 0, 0};
    if (this->IsWindowFrameEnabled())
    {
        windowPixels = this->getWindowPixels();
        if (windowPixels.x != -1 && windowPixels.y != -1)
            pixels = windowPixels;
    }

    if (this->IsObjEnable())
    {
        objPixels = this->getObjectsPixels();
        if (objPixels.x != -1 && objPixels.y != -1)
            pixels = objPixels;
    }

    this->pixel_x = (this->pixel_x + 8) % SCREEN_WIDTH_DEFAULT;
    return pixels;
}

// 1 ppu dot is 4 cpu cycles
void PixelProcessingUnit::run()
{
    while (this->cycles > 0)
    {
        if (this->GetLy() >= SCREEN_HEIGHT_DEFAULT)
        {
            this->runVblankMode();
        }
        else
        {
            if (this->current_dot <= 80)
                this->runOamMode();
            else if (this->current_dot <= 252)
                this->runRenderMode();
            else
                this->runHblankMode();
        }
        // run OAM scan only once, avoid unecessary work
        if (this->mode == OAMScanMode && this->current_dot < 80)
            return;

        // decrement current cycle control
        this->cycles -= 1;
        // every 4cpu ticks, advances 1 dot
        this->current_dot += 1;
        if (this->current_dot == DOTS_PER_LINE)
        {
            this->current_dot = 0;
            this->UpdateLy();
            if (this->GetLy() >= MAX_SCANLINES)
                *this->ly = 0;
        }
    }
}

PixelProcessingUnit *ppu = nullptr;