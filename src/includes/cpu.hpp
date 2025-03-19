#pragma once

#include "definitions.hpp"
#include "instructions.hpp"

class CPU
{
public:
  // define CPU registers
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
  // CPU flags
  bool zero_flag;
  bool carry_flag;
  bool sub_flag;
  bool half_carry_flag;
  // Instruction information + temp data
  InstructionSet current_instruction;
  u8 opcode;
  u16 fetched_data;
  bool prefixed = false;
  // CPU halt state
  bool halt;
  // constructor
  CPU();
};
