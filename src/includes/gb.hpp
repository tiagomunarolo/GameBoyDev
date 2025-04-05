#pragma once
#include "cpu.hpp"
#include "interruption.hpp"
#include "memory.hpp"
#include "serial.hpp"
#include "timer.hpp"
#include "ui.hpp"
#include "ppu.hpp"
#include "dma.hpp"
#include "joypad.hpp"

using namespace std;

class GB
{
private:
  UI *ui_ctx;
  CPU *cpu_ctx;
  PixelProcessingUnit *ppu_ctx;
  Memory *memory_ctx;
  InterruptionContoller *interruption_ctx;
  TimerHanlder *timer_ctx;
  SerialHandler *serial_ctx;
  Joypad *joypad_ctx;
  Dma *dma_ctx;

public:
  GB(const char *rom);
  long step; // internal step (only debugging)
};

extern GB *gb;