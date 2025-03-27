#include "ppu.hpp"
#include "interruption.hpp"

using namespace std;

static const int MAX_SCANLINES = 154;
static const int DOTS_PER_LINE = 456;

static int globalC;

PixelProcessingUnit::PixelProcessingUnit(Memory *mem)
{
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

TileMap PixelProcessingUnit::GetTileMap()
{
    // BIT 6 - window tile map area
    if (*this->lcdc & 0x40)
        // == 1
        return TileMap{0x9C00, 0x9FFF};

    // == 0
    return TileMap{0x9800, 0x9BFF};
}

bool PixelProcessingUnit::IsWindowFrameEnabled()
{
    // BIT 5
    return *this->lcdc & 0x20;
}

BgAddress PixelProcessingUnit::get_tile_range()
{
    // BIT 4
    if (*this->lcdc & 0x10)
        // == 1
        return BgAddress{0x8000, 0x8FFF};

    // == 0
    return BgAddress{0x8800, 0x97FF};
}

BgAddress PixelProcessingUnit::GetBgTileMap()
{
    // BIT 3
    if (*this->lcdc & 0x08)
        // == 1
        return TileMap{0x9C00, 0x9FFF};

    // == 0
    return TileMap{0x9800, 0x9BFF};
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

void PixelProcessingUnit::setCycles(u8 value)
{
    this->cycles += value;
    globalC += value;
}

void PixelProcessingUnit::runOamMode()
{
    this->mode = OAMScanMode;
    *this->stat = ((*this->stat & 0xFC) | 0x2);
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