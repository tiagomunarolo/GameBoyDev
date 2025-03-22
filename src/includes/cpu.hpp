#pragma once

#include "definitions.hpp"
#include "memory.hpp"
#include "instructions.hpp"

class CPU
{

private:
  // main registers
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
  bool ime; // interruption master enable
  // Instruction information + temp data
  InstructionSet current_instruction;
  u8 opcode;
  u16 fetched_data;
  bool prefixed = false;
  // CPU halt state
  bool halt;
  // constructor
  bool running = false;
  // memory reference
  Memory *memory;
  // CPU pipeline cycle
  void fetch_instruction();
  void set_instruction_type();
  void update_current_instruction();
  void fetch_data();
  void execute_instruction();
  void call_interruption();
  void check_timer_overflow();
  void execute_step();

public:
  CPU(Memory *mem);
  // opcode info
  u8 getOpcode();
  void setOpcode(u8 opcode);
  // resgiter info
  u16 getRegister(const Operand &reg);
  void setRegister(const Operand &reg, u16 value);
  // stack info
  u16 *getSP();
  // PC info
  u16 getOldPC();
  u16 getPC();
  void setOldPC(u16 pc);
  void setPC(u16 pc);
  // halted info
  bool isHalted();
  void setHalt(bool status);
  // flags
  bool getFlag(cpuFlags flag);
  void setFlag(cpuFlags flag, bool status);
  // instruction info
  InstructionSet getInstruction();
  void setInstruction(InstructionSet instruction);
  // temp cpu data
  void setFetchedData(u16 value);
  u16 getFetchedData();
  // Memory
  Memory *getMemory();
  // interruption info
  bool getIME();
  void setIME(bool status);
  // run
  void run();
  void stop();
};

extern CPU *cpu; // Definition of global pointer