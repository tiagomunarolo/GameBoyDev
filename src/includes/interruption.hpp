#pragma once
#include "definitions.hpp"
#include "instructions.hpp"
#include "memory.hpp"

enum InterruptionType
{
    JoypadInt = 4,
    Serial = 3,
    Timer = 2,
    LCD = 1,
    VBlank = 0,
    None = 5,
};

class InterruptionContoller
{
private:
    u8 *ie;                       // interruput_enable
    u8 *iflag;                    // interrupt flag
    InterruptionType pending_int; // pending interruption
public:
    InterruptionType getInterruptionType();
    u8 getIE();
    u8 getIF();
    bool hasPendingInterruption();
    void setInterruption(InterruptionType int_type);
    void unsetInterruption();
    Mnemonic getCurrentInterruption();
    InterruptionContoller(Memory *memory);
};

const char *interruption_type_str(InterruptionType in);

extern InterruptionContoller *interruption;