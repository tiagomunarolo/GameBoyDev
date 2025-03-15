#include "timer.hpp"

TimerHanlder::TimerHanlder(Memory *memory)
{
    // This register is incremented at a rate of 16384Hz
    // div -> increment every 64 M-cycles
    this->div_mem = &memory->io[0x04];
    this->tima = &memory->io[0x05];
    this->tma = &memory->io[0x06];
    this->tac = &memory->io[0x07];
    this->div = 0xAB00;
    *this->div_mem = this->div >> 8;
    *this->tima = 0x00;
    *this->tma = 0x00;
    *this->tac = 0xf8;
}

void TimerHanlder::update_timer(int tcycles)
{
    // div is always updated
    u8 old_div = this->div >> 8;
    this->div += tcycles;
    if ((this->div >> 8) != old_div)
        *this->div_mem = this->div >> 8;
    int bit_select = this->check_tima();
    bool update =
        ((1 << bit_select) & old_div) != ((1 << bit_select) & this->div);

    if (bit_select && update)
    {
        if (*this->tima == 0xff)
        {
            *this->tima = *this->tma;
            this->tima_overflow = true;
        }
        else
        {
            *this->tima = *this->tima + 1;
        }
    }
}

// example: 0b 0000 0000 0000 0000

int TimerHanlder::check_tima()
{
    switch (*this->tac & 0x07)
    {
    // bit 2 => enable
    // bit 0-1: clock select
    // returns bit to check on div
    case 0b000:
        return 0;
    case 0b001:
        return 0;
    case 0b010:
        return 0;
    case 0b011:
        return 0;
    case 0b100: // increment every 256 M-cycles
        return 9;
    case 0b101: // increment every 4 M-cycles
        return 3;
    case 0b110: // // increment every 16 M-cycles
        return 5;
    case 0b111: // increment every 64 M-cycles
        return 7;
    default:
        return 0;
    }
}
