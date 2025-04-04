#include "joypad.hpp"
#include "interruption.hpp"

Joypad::Joypad()
{
    this->left = false;
    this->right = false;
    this->up = false;
    this->down = false;
    this->a = false;
    this->b = false;
    this->select = false;
    this->start = false;
}

u8 Joypad::read()
{
    u8 output = 0xCF;

    if (this->selectBtn)
    {
        if (this->start)
        {
            output &= ~(1 << 3);
        }
        if (this->select)
        {
            output &= ~(1 << 2);
        }
        if (this->a)
        {
            output &= ~(1 << 0);
        }
        if (this->b)
        {
            output &= ~(1 << 1);
        }
    }

    if (this->selectDpad)
    {
        if (this->left)
        {
            output &= ~(1 << 1);
        }
        if (this->right)
        {
            output &= ~(1 << 0);
        }
        if (this->up)
        {
            output &= ~(1 << 2);
        }
        if (this->down)
        {
            output &= ~(1 << 3);
        }
    }
    return output;
}

void Joypad::write(u8 value)
{
    this->selectBtn = value & 0x10;
    this->selectDpad = value & 0x20;
}

void Joypad::pressButton(JoypadButton button, bool status)
{

    switch (button)
    {
    case RightBtn:
    {
        this->right = status;
        break;
    }
    case ABtn:
    {
        this->a = status;
        break;
    }
    case LeftBtn:
    {
        this->left = status;
        break;
    }
    case BBtn:
    {
        this->b = status;
        break;
    }
    case UpBtn:
    {
        this->up = status;
        break;
    }
    case SelectBtn:
    {
        this->select = status;
        break;
    }
    case DownBtn:
    {
        this->down = status;
        break;
    }
    case StartBtn:
    {
        this->start = status;
        break;
    }
    default:
        break;
    }
}

Joypad *joypad = nullptr;