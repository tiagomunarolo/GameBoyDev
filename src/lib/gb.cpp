#include "gb.hpp"

// Constructor
GB::GB(const char *rom)
{
    // debug step
    step = 0;
    // memory
    memory = new Memory(rom);
    this->memory_ctx = memory;
    // cpu
    cpu = new CPU(memory);
    this->cpu_ctx = cpu;
    // Setup interruption registers
    interruption = new InterruptionContoller(memory);
    this->interruption_ctx = interruption;
    // timer
    timer = new TimerHanlder(memory);
    this->timer_ctx = timer;
    // serial
    serial = new SerialHandler(memory);
    this->serial_ctx = serial;
    // ppu
    ppu = new PixelProcessingUnit(memory);
    this->ppu_ctx = ppu;
    // ui
    ui = new UI();
    this->ui_ctx = ui;
    // joypad
    joypad = new Joypad();
    this->joypad_ctx = joypad;
}

GB *gb = nullptr;