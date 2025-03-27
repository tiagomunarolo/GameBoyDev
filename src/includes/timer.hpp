#pragma once
#include "definitions.hpp"
#include "memory.hpp"

class TimerHanlder
{
public:
  u16 div;
  u8 *div_mem;
  u8 *tima;
  u8 *tma;
  u8 *tac;
  TimerHanlder(Memory *memory);
  int check_tima();
  void update_timer(int tcycles);
};

extern TimerHanlder *timer;
