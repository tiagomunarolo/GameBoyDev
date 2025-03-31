#pragma once
#include "definitions.hpp"
#include "memory.hpp"

// Screen dimensions
// 160x144 pixels;

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

class Sprite
{
public:
    u8 y_pos;      // byte 0
    u8 x_pos;      // byte 1
    u8 tile_id;    // byte2
    u8 attributes; // byte 3
    Sprite(u8 y_pos, u8 x_pos, u8 tile_id, u8 attributes);
    Sprite()
    {
        Sprite(0, 0, 0, 0);
    };
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
    u8 *wx;
    u8 *wy;
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
    u16 getWindowTileMap();    // Window tile map
    u16 getBgTileMapAddress(); // Bakcgorund Tile Map
    u16 getTileArea();         // Tile area on VRAM
    ObjSize GetObjSize();
    void UpdateLy();
    Sprite sprites[40];
    u8 GetLy();
    u8 getSCX();
    u8 getSCY();
    u8 getWX();
    u8 getWY();
    void setCycles(u8 value);
    u8 getObjPallete(bool bit);
    u8 getBackgroundWindowPallete();
    void run();
};

extern PixelProcessingUnit *ppu;