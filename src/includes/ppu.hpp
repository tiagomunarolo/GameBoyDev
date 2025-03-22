#pragma once
#include "definitions.hpp"
#include "memory.hpp"

// Screen dimensions
// 160x144 pixels;

typedef struct _address
{
    u16 start, end;
} BgAddress, TileMap;

enum PpuMode
{
    HBlankMode = 0,
    VBlankMode = 1,
    OAMScanMode = 2,
    RenderingMode = 3,
};

enum ObjSize
{
    _8x8 = 0,
    _16x16 = 1,
};

class PixelProcessingUnit
{
private:
    Memory *mem;
    u8 *lcdc;
    u8 *stat;
    u8 *scy;
    u8 *scx;
    u8 *ly;
    u8 *lyc;
    u8 *bgp;
    u8 *obp0;
    u8 *obp01;

public:
    // Each tile occupies 16 bytes, where each line is represented by 2 bytes:

    PixelProcessingUnit(Memory *mem);
    bool lcd_is_on();
    bool window_frame_enabled();
    bool obj_enable();
    bool bg_window_enable_priority();
    PpuMode get_ppu_mode();
    TileMap get_tile_map();
    BgAddress get_tile_range();
    BgAddress get_bg_tile_map();
    ObjSize get_obj_size();
    u8 get_scx();
    u8 get_scy();
};

extern PixelProcessingUnit *ppu;