#include "interruption.hpp"
#include "gb.hpp"
#include "stack.hpp"

using namespace std;

InterruptionContoller::InterruptionContoller(Memory *memory)
{
    this->ime_flag = new bool();
    *this->ime_flag = false;
    this->ie = &memory->ie;
    this->iflag = &memory->iflags;
    *this->ie = 0x00;
    *this->iflag = 0xe1;
};

InterruptionType InterruptionContoller::get_interruption_type()
{
    u8 flags = *this->ie & *this->iflag;
    if (flags & 0b01)
        return VBlank;
    else if (flags & 0b10)
        return LCD;
    else if (flags & 0b100)
        return Timer;
    else if (flags & 0b1000)
        return Serial;
    else
        return Joypad;
}

const char *interruption_type_str(InterruptionType in)
{
    switch (in)
    {
    case VBlank:
        return "VBlank";
        break;
    case LCD:
        return "LCD";
        break;
    case Timer:
        return "Timer";
        break;
    case Serial:
        return "Serial";
        break;
    case Joypad:
        return "Joypad";
        break;
    default:
        return "Unknown";
        break;
    }
}

void InterruptionContoller::set_interruption(InterruptionType int_type)
{
    switch (int_type)
    {
    case VBlank:
    {
        *this->ie = *this->ie | 0b00001;
        *this->iflag = *this->iflag | 0b00001;
        break;
    }
    case LCD:
    {
        *this->ie = *this->ie | 0b00010;
        *this->iflag = *this->iflag | 0b00010;
        break;
        break;
    }
    case Timer:
    {
        *this->ie = *this->ie | 0b00100;
        *this->iflag = *this->iflag | 0b00100;
        break;
        break;
    }
    case Serial:
    {
        *this->ie = *this->ie | 0b01000;
        *this->iflag = *this->iflag | 0b01000;
        break;
        break;
    }
    case Joypad:
    {
        *this->ie = *this->ie | 0b1000;
        *this->iflag = *this->iflag | 0b10000;
        break;
        break;
    }
    default:
        break;
    }
}

Mnemonic InterruptionContoller::getTnterruptionMnemonic(InterruptionType in)
{
    switch (in)
    {
    case VBlank:
        return INT_VBLANK;
        break;
    case LCD:
        return INT_LCD;
        break;
    case Timer:
        return INT_TIMER;
        break;
    case Serial:
        return INT_SERIAL;
        break;
    case Joypad:
        return INT_JOYPAD;
        break;
    default:
        return INT_UNKNOWN;
        break;
    }
}

void InterruptionContoller::check_interruption()
{
    if (*this->ime_flag && (*this->ie & *this->iflag) != 0x00)
    {
        InterruptionType interruption = get_interruption_type();
        const char *int_type = interruption_type_str(interruption);
        cout << "Should execute: " << int_type << endl;
        // disbale ime flag
        *this->ime_flag = false;
        // disbale current interruption
        *this->iflag = (*this->iflag) & ~(1 << int(interruption));
        this->pending_int = interruption;
    }
};
