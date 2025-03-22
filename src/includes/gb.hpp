#pragma once
#include "cpu.hpp"
#include "interruption.hpp"
#include "memory.hpp"
#include "serial.hpp"
#include "timer.hpp"
#include "ui.hpp"
#include "ppu.hpp"

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

public:
  GB(const char *rom);
  UI *get_ui();
  CPU *get_cpu();
  Memory *get_memory();
  InterruptionContoller *get_interruption();
  TimerHanlder *get_timer();
  long step; // internal step (only debugging)
  void stop_cpu();
};

extern GB *gb;