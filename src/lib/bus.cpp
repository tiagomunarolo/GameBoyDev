#include "bus.hpp"

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

using namespace std;

u8 read_u8bit_address(u16 address, Memory *mem)
{
    if (address <= 0x3FFF)
    { // rom bank 0
        return mem->rom[address];
    }
    else if (address <= 0x7FFF)
    { // rom bank 1 switchable
        u8 current_bank = mem->rom_bank1;
        u16 addr = (current_bank * 0x4000) + (address - 0x4000);
        return mem->rom[addr];
    }
    else if (address <= 0x9FFF)
    { // video ram
        throw std::runtime_error("INVALID ADDRESS RANGE: vram");
    }
    else if (address <= 0xBFFF)
    { // 8 KiB External RAM
        throw std::runtime_error("INVALID ADDRESS RANGE: external ram");
    }
    else if (address <= 0xCFFF)
    { // wram 0
        return mem->wram0[address - 0xC000];
    }
    else if (address <= 0xDFFF)
    { // wram 1
        return mem->wram1[mem->current_wram1][address - 0xD000];
    }
    else if (address <= 0xFDFF)
    { // echo ram
        return mem->wram0[address - 0xC000];
    }
    else if (address <= 0xFE9F)
    { // OAM
        throw std::runtime_error("INVALID ADDRESS RANGE: oam");
    }
    else if (address <= 0xFEFF)
    { // prohibited
        throw std::runtime_error("INVALID ADDRESS RANGE: <prohibited area>");
    }
    else if (address <= 0xFF7F)
    { // I/O registers
        if (address == 0xff0f)
            return mem->iflags;
        return mem->io[address - 0xFF00];
    }
    else if (address >= 0xFF80 && address <= 0xFFFE)
    { // hram
        return mem->hiram[address - 0xFF80];
    }
    else if (address == 0xFFFF)
        return mem->ie;

    throw std::runtime_error("Interrupt read");
}

u16 read_u16bit_address(u16 address, Memory *mem)
{
    u8 lo = read_u8bit_address(address, mem);
    u8 hi = read_u8bit_address(address + 0x1, mem);
    return (u16)((hi << 8) | lo);
}

int8_t read_8bit_address(u16 address, Memory *mem)
{
    return (int8_t)read_u8bit_address(address, mem);
}

void bus_write(u16 address, u8 value, Memory *mem)
{
    if (address <= 0x3FFF)
    { // rom bank 0
        throw std::runtime_error("INVALID RANGE: <ROM_BANK 0 (RO) >");
        return;
    }
    else if (address <= 0x7FFF)
    { // rom bank 1 switchable
        // select current bannk
        mem->rom_bank1 = 0b11111 & value;
        return;
    }
    else if (address <= 0x9FFF)
    { // video ram
        mem->vram[address - 0x8000] = value;
        return;
    }
    else if (address <= 0xBFFF)
    { // 8 KiB External RAM
        throw std::runtime_error("INVALID ADDRESS RANGE: unimplemented");
        return;
    }
    else if (address <= 0xCFFF)
    { // wram 0
        mem->wram0[address - 0xC000] = value;
        return;
    }
    else if (address <= 0xDFFF)
    { // wram 1
        mem->wram1[mem->current_wram1][address - 0xD000] = value;
        return;
    }
    else if (address <= 0xFDFF)
    { // echo ram
        mem->wram0[address - 0xC000] = value;
        return;
    }
    else if (address <= 0xFE9F)
    { // OAM
        throw std::runtime_error("INVALID ADDRESS RANGE: <oam>");
        return;
    }
    else if (address <= 0xFEFF)
    { // prohibited
        throw std::runtime_error("INVALID ADDRESS RANGE: <prohibited area>");
    }
    else if (address <= 0xFF7F)
    { // I/O registers
        // Writing any value to this register resets it to $00
        if (address == 0xff04)
        { // timer div
            mem->io[0x4] = 0x00;
        }
        else if (address == 0xff0f)
        {
            mem->iflags = value;
        }
        else if (address == 0xff70)
        { // wram1 select
            mem->current_wram1 = value;
        }
        else
        {
            mem->io[address - 0xFF00] = value;
        }
        return;
    }
    else if (address >= 0xFF80 && address <= 0xFFFE)
    { // hram
        mem->hiram[address - 0xFF80] = value;
        return;
    }
    else
    {
        mem->ie = value;
    }
}

void bus_write16(u16 address, u16 value, Memory *mem)
{
    u8 high = value >> 8;
    u8 low = value & 0xff;
    if ((address & 0xff) == 0x00)
    {
        bus_write(address + 1, high, mem);
        bus_write(address, low, mem);
    }
    else
    {
        bus_write(address + 1, high, mem);
        bus_write(address, low, mem);
    }
}
