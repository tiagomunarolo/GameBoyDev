#include "ppu.hpp"
#include "ui.hpp"
#include "interruption.hpp"
#include <thread>
#include <chrono>

using namespace std;

static const int MAX_SCANLINES = 154;
static const int DOTS_PER_LINE = 456;
static int vblank_lines = 0;

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