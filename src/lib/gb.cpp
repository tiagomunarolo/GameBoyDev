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
}

// Returns a pointer to the UI instance
UI *GB::get_ui() { return this->ui_ctx; }

// Returns a pointer to the CPU instance
CPU *GB::get_cpu() { return this->cpu_ctx; }

// Returns a pointer to the Memory instance
Memory *GB::get_memory() { return this->memory_ctx; }

// Returns a pointer to the InterruptionController instance
InterruptionContoller *GB::get_interruption() { return this->interruption_ctx; }

// Returns a pointer to the TimerHandler instance
TimerHanlder *GB::get_timer() { return this->timer_ctx; }

// stop cpu
void GB::stop_cpu() { this->cpu_ctx->stop(); };

GB *gb = nullptr;