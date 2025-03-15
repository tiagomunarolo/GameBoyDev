#pragma once
#include "definitions.hpp"

// 0x0000 - 0x3FFF : ROM Bank 0
// 0x4000 - 0x7FFF : ROM Bank 1 - Switchable
// 0x8000 - 0x9FFF : VRAM
// 0x9800 - 0x9BFF : BG Map 1
// 0x9C00 - 0x9FFF : BG Map 2
// 0xA000 - 0xBFFF : Cartridge RAM
// 0xC000 - 0xCFFF : RAM Bank 0
// 0xD000 - 0xDFFF : RAM Bank 1-7 - switchable - Color only
// 0xE000 - 0xFDFF : Reserved - Echo RAM
// 0xFE00 - 0xFE9F : Object Attribute Memory
// 0xFEA0 - 0xFEFF : Reserved - Unusable
// 0xFF00 - 0xFF7F : I/O Registers
// 0xFF80 - 0xFFFE : Zero Page

class Memory
{

public:
    const char *rom;
    u8 current_wram1;
    u8 vram[0x2000];
    u8 external_ram[0x2000];
    u8 wram0[0x1000];
    u8 **wram1; // wram banks 1-7
    u8 oam[0xA0];
    u8 io[0x80];
    u8 hiram[0x80];
    u8 rom_bank1 = 1;
    u8 iflags = 0xe1; // ff0f
    u8 ie = 0x00;     // interruption enable ffff

    Memory(const char *rom);
};
