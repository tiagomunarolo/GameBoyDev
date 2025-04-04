#pragma once
#include "definitions.hpp"

enum JoypadButton
{
    UpBtn,
    DownBtn,
    LeftBtn,
    RightBtn,
    ABtn,
    BBtn,
    SelectBtn,
    StartBtn,
};

class Joypad
{
private:
    bool start;
    bool select;
    bool a;
    bool b;
    bool up;
    bool down;
    bool left;
    bool right;
    bool selectBtn;
    bool selectDpad;

public:
    Joypad();
    u8 read();
    void write(u8 value);
    void pressButton(JoypadButton button, bool status);
};

extern Joypad *joypad;