#include "interruption.hpp"
#include "stack.hpp"

using namespace std;

InterruptionContoller::InterruptionContoller(Memory *memory)
{
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
        *this->iflag = *this->iflag | 0b00001;
        break;
    }
    case LCD:
    {
        *this->iflag = *this->iflag | 0b00010;
        break;
    }
    case Timer:
    {
        *this->iflag = *this->iflag | 0b00100;
        break;
    }
    case Serial:
    {
        *this->iflag = *this->iflag | 0b01000;
        break;
    }
    case Joypad:
    {
        *this->iflag = *this->iflag | 0b10000;
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

bool InterruptionContoller::hasPendingInterruption()
{
    if ((*this->ie & *this->iflag) != 0x00)
    {
        InterruptionType interruption = get_interruption_type();
        const char *int_type = interruption_type_str(interruption);
        cout << "Should execute: " << int_type << endl;
        // disbale current interruption
        *this->iflag = (*this->iflag) & ~(1 << int(interruption));
        this->pending_int = interruption;
        return true;
    }
    return false;
};

InterruptionContoller *interruption = nullptr;
