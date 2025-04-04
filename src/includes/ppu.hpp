#pragma once
#include <vector>
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

typedef struct Pixels
{
    int x = 0;               // x position
    int y = 0;               // y position
    unsigned long color;     // pixel color
    bool bg_window_priority; // bg has higher priority?
} Pixels;

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
    u8 internalWy;
    PpuMode mode;
    int cycles;
    u16 current_dot;
    int pixel_x;
    u8 getObjectSize();
    void updateLy();
    Sprite sprites[40];
    // position lcd registers
    u8 getSCX();
    u8 getSCY();
    u8 getWX();
    u8 getWY();
    // Palletes info
    u8 getObjPallete(bool bit);
    u8 getBackgroundWindowPallete();
    // Flags related to window, bg and oam frames
    bool IsWindowFrameEnabled();
    bool IsObjEnable();
    bool BgWindowEnablePriority();
    //  Tile map information
    u16 getWindowTileMap();    // Window tile map
    u16 getBgTileMapAddress(); // Bakcgorund Tile Map
    u16 getTileArea();         // Tile area on VRAM
    // Pixel fecth
    Pixels getObjectsPixels(int y);
    Pixels getBackgroundPixels(int y);
    Pixels getWindowPixels(int y);
    // PPU modes
    void runOamMode();
    void runRenderMode();
    void runHblankMode();
    void runVblankMode();

public:
    // Constructor
    PixelProcessingUnit(Memory *mem);
    // main method to execute ppu
    void run();
    // Flag to indicate if LCD is enabled
    bool IsLcdOn();
    // Method to fetch pixels on current row
    std::vector<Pixels> getPixels(int row);
    // Check ppu execution mode (view PpuMode)
    PpuMode GetPpuMode();
    // Sync PPU cycles with internal timer
    void setCycles(u8 value);
    // flag to allow Rendering
    bool isRendering();
    // unset rendering flag
    void finishRendering();
    // Get current LY
    u8 getLy();
};

extern PixelProcessingUnit *ppu;