#include "ppu.hpp"
#include "ui.hpp"
#include "interruption.hpp"

using namespace std;

static const int MAX_SCANLINES = 154;
static const int DOTS_PER_LINE = 456;

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

ObjSize PixelProcessingUnit::GetObjSize()
{
    // BIT 2
    if (*this->lcdc & 0x04)
        // == 1
        return _16x16;

    // == 0
    return _8x8;
}

void PixelProcessingUnit::UpdateLy()
{
    if (this->IsLcdOn() && this->GetLy() >= 0 && this->GetLy() <= 143)
        ui->update();
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
    // FE00	FE9F	Object attribute memory (OAM)
    for (int i = 0; i <= 40; i++)
    {
        this->sprites[i].y_pos = mem->oam[4 * i];
        this->sprites[i].x_pos = mem->oam[4 * i + 1];
        this->sprites[i].tile_id = mem->oam[4 * i + 2];
        this->sprites[i].attributes = mem->oam[4 * i + 3];
    }
}

void PixelProcessingUnit::runRenderMode()
{
    this->mode = RenderingMode;
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
// 1 ppu dot is 4 cpu cycles
void PixelProcessingUnit::run()
{
    while (this->cycles > 0)
    {
        if (this->GetLy() >= 144)
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