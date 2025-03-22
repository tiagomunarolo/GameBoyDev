#include "ppu.hpp"

PixelProcessingUnit::PixelProcessingUnit(Memory *mem)
{
    // LCD
    this->mem = mem;
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

PpuMode PixelProcessingUnit::get_ppu_mode()
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

bool PixelProcessingUnit::lcd_is_on()
{
    // BIT 7 indicates if lcd is on on lcdc register
    return *this->lcdc & 0x80;
}

TileMap PixelProcessingUnit::get_tile_map()
{
    // BIT 6 - window tile map area
    if (*this->lcdc & 0x40)
        // == 1
        return TileMap{0x9C00, 0x9FFF};

    // == 0
    return TileMap{0x9800, 0x9BFF};
}

bool PixelProcessingUnit::window_frame_enabled()
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

BgAddress PixelProcessingUnit::get_bg_tile_map()
{
    // BIT 3
    if (*this->lcdc & 0x08)
        // == 1
        return TileMap{0x9C00, 0x9FFF};

    // == 0
    return TileMap{0x9800, 0x9BFF};
}

ObjSize PixelProcessingUnit::get_obj_size()
{
    // BIT 2
    if (*this->lcdc & 0x04)
        // == 1
        return _16x16;

    // == 0
    return _8x8;
}

bool PixelProcessingUnit::obj_enable()
{
    // BIT 1
    return *this->lcdc & 0b10;
}

bool PixelProcessingUnit::bg_window_enable_priority()
{
    // BIT 0
    return *this->lcdc & 0b01;
}

u8 PixelProcessingUnit::get_scx()
{
    return *this->scx;
}

u8 PixelProcessingUnit::get_scy()
{
    return *this->scy;
}

PixelProcessingUnit *ppu = nullptr;