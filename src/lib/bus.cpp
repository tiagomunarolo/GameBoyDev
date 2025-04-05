#include "bus.hpp"
#include "ppu.hpp"
#include "timer.hpp"
#include "dma.hpp"
#include "joypad.hpp"
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

u8 read_u8bit_address(u16 address)
{

    // if (dma->active && address < 0xff80)
    //     return 0x00;

    if (address >= 0 && address <= 0x3FFF)
    { // rom bank 0
        return memory->rom[address];
    }
    else if (address >= 0x4000 && address <= 0x7FFF)
    { // rom bank 1 switchable
        u8 current_bank = memory->rom_bank1;
        u16 addr = (current_bank * 0x4000) + (address - 0x4000);
        return memory->rom[addr];
    }
    else if (address >= 0x8000 && address <= 0x9FFF)
    { // video ram
        return memory->vram[address - 0x8000];
    }
    else if (address >= 0xA000 && address <= 0xBFFF)
    {
        if (!memory->ram_enable)
            return 0xff;
        return memory->external_ram[address - 0xA000];
    }
    else if (address >= 0xC000 && address <= 0xCFFF)
    { // wram 0
        return memory->wram0[address - 0xC000];
    }
    else if (address >= 0xD000 && address <= 0xDFFF)
    { // wram 1
        return memory->wram1[memory->current_wram1][address - 0xD000];
    }
    else if (0xE000 <= address && address <= 0xFDFF)
    { // echo ram
        return memory->wram0[address - 0xC000];
    }
    else if (address >= 0xFE00 && address <= 0xFE9F)
    { // OAM
        return memory->oam[address - 0xfe00];
    }
    else if (address >= 0xFEA0 && address <= 0xFEFF)
    { // prohibited
        return 0x00;
    }
    else if (address >= 0xFF00 && address <= 0xFF7F)
    { // I/O registers
        if (address == 0xff00)
            return joypad->read();
        else if (address == 0xff0f)
            return memory->iflags;
        else
            return memory->io[address - 0xFF00];
    }
    else if (address >= 0xFF80 && address <= 0xFFFE)
    { // hram
        return memory->hiram[address - 0xFF80];
    }
    else if (address == 0xFFFF)
        return memory->ie;

    throw std::runtime_error("Error reading invalid address");
}

u16 read_u16bit_address(u16 address)
{
    u8 lo = read_u8bit_address(address);
    u8 hi = read_u8bit_address(address + 0x1);
    return (u16)((hi << 8) | lo);
}

int8_t read_8bit_address(u16 address)
{
    return (int8_t)read_u8bit_address(address);
}

void bus_write(u16 address, u8 value)
{
    // if (dma->active && address < 0xff80)
    //     return;
    if (address >= 0 && address <= 0x7FFF)
    {
        // select current rom bank 1
        if (0x0000 <= address && address <= 0x1FFF) // 0000–1FFF — RAM enable
        {
            memory->ram_enable = (value & 0x0f) == 0xA;
            return;
        }
        else if (0x2000 <= address && address <= 0x3FFF)
        {                                   // 2000–3FFF — ROM Bank Number (Write Only)
            value = value == 0 ? 1 : value; // bank 0 is mapped to 1
            memory->rom_bank1 = 0b11111 & value;
            return;
        }
        else
        {
            printf("UNHALLED WRITE: address=0x%.4X, value=0x%.2X\n", address, value);
        }
    }
    else if (address >= 0x8000 && address <= 0x9FFF)
    { // video ram
        memory->vram[address - 0x8000] = value;
        return;
    }
    else if (address >= 0xA000 && address <= 0xBFFF)
    { // 8 KiB External RAM

        memory->external_ram[address - 0xA000] = value;
        return;
    }
    else if (address >= 0xC000 && address <= 0xCFFF)
    { // wram 0
        memory->wram0[address - 0xC000] = value;
        return;
    }
    else if (address >= 0xD000 && address <= 0xDFFF)
    { // wram 1
        memory->wram1[memory->current_wram1][address - 0xD000] = value;
        return;
    }
    else if (address >= 0xE000 && address <= 0xFDFF)
    { // echo ram
        memory->wram0[address - 0xC000] = value;
        return;
    }
    else if (address >= 0xFE00 && address <= 0xFE9F)
    { // OAM
        dma->write(address, value);
        return;
    }
    else if (address >= 0xFEA0 && address <= 0xFEFF)
    { // prohibited
        // printf("INVALID ADDRESS WRITE: <prohibited area> [%.2x]\n", address);
        return;
    }
    else if (address >= 0xFF00 && address <= 0xFF7F)
    { // I/O registers
        if (address == 0xff00)
            joypad->write(value);
        // Writing any value to this register resets it to $00
        else if (address == 0xff04)
        { // timer div
            memory->io[0x4] = 0x00;
        }
        else if (address == 0xff0f)
        {
            memory->iflags = value;
        }
        else if (address == 0xff70)
        { // wram1 select
            memory->current_wram1 = value;
        }
        else if (address == 0xff46)
        { // // DMA OAM transfer
            memory->io[address - 0xFF00] = value;
            dma->setActive(); // 640 cpu ticks (160m CPU cycles)
        }
        else
        {
            memory->io[address - 0xFF00] = value;
        }
        return;
    }
    else if (address >= 0xFF80 && address <= 0xFFFE)
    { // hram
        memory->hiram[address - 0xFF80] = value;
        return;
    }
    else if (address == 0xffff)
    {
        memory->ie = value;
    }
}

void bus_write16(u16 address, u16 value)
{
    u8 high = value >> 8;
    u8 low = value & 0xff;
    bus_write(address + 1, high);
    bus_write(address, low);
}
