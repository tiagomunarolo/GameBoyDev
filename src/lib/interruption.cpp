#include "interruption.hpp"
#include "stack.hpp"

using namespace std;

InterruptionContoller::InterruptionContoller(Memory *memory)
{
    this->ie = &memory->ie;
    this->iflag = &memory->iflags;
    this->pending_int = None;
    *this->ie = 0x00;
    *this->iflag = 0xe1;
};

InterruptionType InterruptionContoller::getInterruptionType()
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

u8 InterruptionContoller::getIE()
{
    return *this->ie;
}
u8 InterruptionContoller::getIF()
{
    return *this->iflag;
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

void InterruptionContoller::unsetInterruption()
{
    switch (this->pending_int)
    {
    case VBlank:
    {
        *this->iflag = *this->iflag & ~0b00001;
        break;
    }
    case LCD:
    {
        *this->iflag = *this->iflag & ~0b00010;
        break;
    }
    case Timer:
    {
        *this->iflag = *this->iflag & ~0b00100;
        break;
    }
    case Serial:
    {
        *this->iflag = *this->iflag & ~0b01000;
        break;
    }
    case Joypad:
    {
        *this->iflag = *this->iflag & ~0b10000;
        break;
    }
    default:
        break;
    }
}

void InterruptionContoller::setInterruption(InterruptionType int_type)
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

Mnemonic InterruptionContoller::getCurrentInterruption()
{
    switch (this->pending_int)
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
    if ((this->getIE() & this->getIF()) != 0x00)
    {
        InterruptionType interruption = getInterruptionType();
        const char *int_type = interruption_type_str(interruption);
        InterruptionType old_pending_int = this->pending_int;
        this->pending_int = interruption;
        if (this->pending_int != old_pending_int)
            cout << "Interruption Flags Added: " << int_type << endl;

        return true;
    }
    // no pending interruption
    this->pending_int = None;
    return false;
};

InterruptionContoller *interruption = nullptr;
