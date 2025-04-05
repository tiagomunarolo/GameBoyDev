#pragma once
#include "memory.hpp"

class Dma
{
private:
    int delay;
    int current; // current transfer
    int cycles;  //  timer ticks

public:
    bool active;
    u8 read(u16 address);
    void write(u16 address, u8 value);
    void setActive();
    void setCycles(int cycles);
    void run();
    Dma();
};

extern Dma *dma;