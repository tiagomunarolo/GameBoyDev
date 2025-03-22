#pragma once
#include "definitions.hpp"
#include "instructions.hpp"
#include "memory.hpp"

enum InterruptionType
{
    Joypad = 4,
    Serial = 3,
    Timer = 2,
    LCD = 1,
    VBlank = 0,
    None = 5,
};

class InterruptionContoller
{
public:
    u8 *ie;    // interruput_enable
    u8 *iflag; // interrupt flag
    InterruptionType get_interruption_type();
    bool hasPendingInterruption();
    void set_interruption(InterruptionType int_type);
    InterruptionType pending_int = None;
    Mnemonic getTnterruptionMnemonic(InterruptionType int_type);
    InterruptionContoller(Memory *memory);
};

const char *interruption_type_str(InterruptionType in);

extern InterruptionContoller *interruption;