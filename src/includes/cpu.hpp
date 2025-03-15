#pragma once

#include "definitions.hpp"
#include "instructions.hpp"

class CPU
{
public:
  reg a;
  reg b;
  reg c;
  reg d;
  reg e;
  reg f;
  reg h;
  reg l;
  u16 sp;
  u16 old_pc;
  u16 pc;
  bool zero_flag;
  bool carry_flag;
  bool sub_flag;
  bool half_carry_flag;
  u8 opcode;
  u16 fetched_data;
  bool prefixed = false;
  bool halt;
  InstructionSet current_instruction;

  CPU();
};
