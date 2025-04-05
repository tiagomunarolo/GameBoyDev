#include "dma.hpp"
#include "bus.hpp"

Dma::Dma()
{
    this->active = false;
    this->current = 0;
    this->delay = 2;
    this->cycles = 0;
}

void Dma::setCycles(int cycles)
{
    if (this->active)
        this->cycles += cycles;
}

u8 Dma::read(u16 addr)
{
    if (this->active)
        return 0xFF;
    return memory->oam[addr - 0xfe00];
}

void Dma::write(u16 addr, u8 value)
{
    if (this->active)
        return;
    memory->oam[addr - 0xfe00] = value;
}

void Dma::run()
{
    if (!active)
        return;

    if (this->delay)
    {
        this->delay--;
        return;
    }

    u16 base_addr = memory->io[0x46] << 8;
    while (this->cycles >= 4)
    {
        memory->oam[this->current] = read_u8bit_address(base_addr + current);
        current++;
        this->cycles -= 4;
    }

    this->active = this->current < 160;
}

void Dma::setActive()
{
    if (this->active)
        return;

    this->active = true;
    this->delay = 2;
    this->current = 0;
    this->cycles = 0;
}

Dma *dma = nullptr;