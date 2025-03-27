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
    PpuMode mode;
    int cycles;
    u16 current_dot;
    void runOamMode();
    void runRenderMode();
    void runHblankMode();
    void runVblankMode();

public:
    // Each tile occupies 16 bytes, where each line is represented by 2 bytes:

    PixelProcessingUnit(Memory *mem);
    bool IsLcdOn();
    bool IsWindowFrameEnabled();
    bool IsObjEnable();
    bool BgWindowEnablePriority();
    PpuMode GetPpuMode();
    TileMap GetTileMap();
    BgAddress get_tile_range();
    BgAddress GetBgTileMap();
    ObjSize GetObjSize();
    void UpdateLy();
    u8 GetLy();
    u8 getSCX();
    u8 getSCY();
    void setCycles(u8 value);
    void run();
};

extern PixelProcessingUnit *ppu;