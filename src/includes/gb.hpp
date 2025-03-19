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
  UI *ui;
  CPU *cpu;
  PixelProcessingUnit *ppu;
  Memory *memory;
  InterruptionContoller *interruption;
  TimerHanlder *timer;
  SerialHandler *serial;
  std::atomic<bool> run; // thread controller

public:
  GB(const char *rom);
  UI *get_ui();
  CPU *get_cpu();
  Memory *get_memory();
  InterruptionContoller *get_interruption();
  TimerHanlder *get_timer();
  long step; // internal step (only debugging)
  void check_halt();
  void call_interruption();
  void check_interruption();
  void check_timer_overflow();
  const char *serial_output();
  void fetch_instruction();
  void fetch_data();
  void execute_instruction();
  void set_instruction_type();
  void update_current_instruction();
  void execute_step();
  std::atomic<bool> thread_run();
  void kill();
};
