#include "cpu.hpp"
#include "bus.hpp"
#include "processor.hpp"
#include "fetch_data.hpp"
#include "mnemonics.hpp"
#include "interruption.hpp"
#include "serial.hpp"
#include "timer.hpp"
#include "ppu.hpp"
#include <chrono>
#include <thread>

#ifdef DEBUG_CPU
#include "debug.hpp"
#endif

using namespace std;

CPU::CPU(Memory *mem)
    : a(0x01), b(0x00), c(0x13), d(0x00), e(0xD8),
      f(0xB0), h(0x01), l(0x4D), sp(0xFFFE), pc(0x0100), ime(false), opcode(0), fetched_data(0),
      halt(false), memory(mem) {};

u8 CPU::getOpcode()
{
  return this->opcode;
}

void CPU::setOpcode(u8 opcode)
{
  this->opcode = opcode;
}
u16 CPU::getRegister(const Operand &reg)
{
  auto regValue = std::get_if<Registers>(&reg);
  if (!regValue)
    throw std::runtime_error("Not a register!");
  switch (*regValue)
  {
  case A:
    return this->a;
  case F:
    return this->f;
  case B:
    return this->b;
  case C:
    return this->c;
  case D:
    return this->d;
  case E:
    return this->e;
  case L:
    return this->l;
  case H:
    return this->h;
  case AF:
    return (this->a << 8) | this->f;
  case BC:
    return (this->b << 8) | this->c;
  case DE:
    return (this->d << 8) | this->e;
  case HL:
    return (this->h << 8) | this->l;
  case SP:
    return this->sp;
  }
}
void CPU::setRegister(const Operand &regValue, u16 value)
{
  auto reg = std::get_if<Registers>(&regValue);
  if (!reg)
    throw std::runtime_error("Not a register!");
  if (*reg == A)
    this->a = (u8)(value & 0x00ff);
  else if (*reg == B)
    this->b = (u8)(value & 0x00ff);
  else if (*reg == C)
    this->c = (u8)(value & 0x00ff);
  else if (*reg == D)
    this->d = (u8)(value & 0x00ff);
  else if (*reg == E)
    this->e = (u8)(value & 0x00ff);
  else if (*reg == F)
    this->f = (u8)(value & 0x00ff);
  else if (*reg == H)
    this->h = (u8)(value & 0x00ff);
  else if (*reg == L)
    this->l = (u8)(value & 0x00ff);
  else if (*reg == AF)
  {
    this->a = value >> 8;
    this->f = 0xF0 & value;
  }
  else if (*reg == BC)
  {
    this->b = value >> 8;
    this->c = 0xFF & value;
  }
  else if (*reg == HL)
  {
    this->h = value >> 8;
    this->l = 0xFF & value;
  }
  else if (*reg == DE)
  {
    this->d = value >> 8;
    this->e = 0xFF & value;
  }
  else if (*reg == SP)
  {
    this->sp = value;
  }
  else
  {
    throw std::runtime_error("Invalid Register");
  }
}
u16 CPU::getPC()
{
  return this->pc;
}
void CPU::setPC(u16 pc)
{
  this->pc = pc;
}

u16 *CPU::getSP()
{
  return &this->sp;
}

u16 CPU::getOldPC()
{
  return this->old_pc;
}
void CPU::setOldPC(u16 pc)
{
  this->old_pc = pc;
}
bool CPU::isHalted()
{
  return this->halt;
}
void CPU::setHalt(bool status)
{
  this->halt = status;
}

bool CPU::getFlag(cpuFlags flag)
{
  switch (flag)
  {
  case ZERO_FLAG:
    return this->getRegister(F) & (1 << 7);
  case CARRY_FLAG:
    return this->getRegister(F) & (1 << 4);
  case HC_FLAG:
    return this->getRegister(F) & (1 << 5);
  case SUB_FLAG:
    return this->getRegister(F) & (1 << 6);
  default:
    throw std::runtime_error("Invalid required CPU flag");
  }
}

void CPU::setFlag(cpuFlags flag, bool status)
{
  u8 flags = this->getRegister(F);

  if (flag == ZERO_FLAG)
  {
    if (status)
      flags |= (1 << 7);
    else
      flags &= ~(1 << 7);
  }
  else if (flag == SUB_FLAG)
  {
    if (status)
      flags |= (1 << 6);
    else
      flags &= ~(1 << 6);
  }
  else if (flag == HC_FLAG)
  {
    if (status)
      flags |= (1 << 5);
    else
      flags &= ~(1 << 5);
  }
  else if (flag == CARRY_FLAG)
  {
    if (status)
      flags |= (1 << 4);
    else
      flags &= ~(1 << 4);
  }

  this->setRegister(F, flags);
}

InstructionSet CPU::getInstruction()
{
  return this->current_instruction;
}

void CPU::setInstruction(InstructionSet instruction)
{
  this->current_instruction = instruction;
}

void CPU::setFetchedData(u16 value)
{
  this->fetched_data = value;
};
u16 CPU::getFetchedData()
{
  return this->fetched_data;
}

void CPU::stop()
{
  this->running = false;
}

bool CPU::getIME()
{
  return this->ime;
}
void CPU::setIME(bool status)
{
  this->ime = status;
}

// Determines the type of instruction being executed
void CPU::set_instruction_type()
{
  // Read the opcode at the current program counter
  this->setOpcode(read_u8bit_address(this->getPC()));
  // Check if it is a prefixed instruction
  this->prefixed = this->opcode == 0xcb;
  if (!this->prefixed)
    return;
  // Prefixed instructions are 2 bytes long, increment PC and read the new
  // opcode
  this->setPC(this->getPC() + 1);
  this->setOpcode(read_u8bit_address(this->getPC()));
}

void CPU::update_current_instruction()
{
  this->set_instruction_type();
  InstructionType type =
      this->prefixed ? PREFIXED_TYPE : NON_PREFIXED_TYPE;
  this->setInstruction(
      get_instruction_by_opcode(this->getOpcode(), type));
}
void CPU::fetch_instruction()
{
  // Store the old program counter (useful for debugging)
  this->setOldPC(this->pc);
  // Update the current instruction
  this->update_current_instruction();
  // Move to the next instruction in memory
  this->setPC(this->getPC() + 1);
}

// Fetches required data for the current instruction
void CPU::fetch_data()
{
  u8 size = this->getInstruction().bytes;
  fetch_data_and_update_registers();

  if (this->prefixed)
    return;

  // Move PC forward to account for the instruction's size
  u16 newPC = this->getPC() + (size - 1);
  this->setPC(newPC);
}

// Executes the currently loaded instruction
void CPU::execute_instruction()
{
  // Retrieve the function corresponding to the current instruction mnemonic
  ProcessorFunc func = get_processor(this->getInstruction().mnemonic);

  // Check for any pending interruptions before executing
  if (this->getIME() && interruption->hasPendingInterruption())
  {
    this->setIME(false);
    interruption->unsetInterruption();
  }

  // If no valid function is found, throw an error
  if (!func)
    throw std::runtime_error("Unknown operation");

  // Execute the retrieved function
  func();
}

// Handles pending interruptions and executes the corresponding instruction
void CPU::call_interruption()
{
  // Save the current instruction state
  InstructionSet saved = cpu->current_instruction;
  u16 savedData = getFetchedData();
  // update mnemonic to current interruption
  this->current_instruction.mnemonic = interruption->getCurrentInterruption();
  // Execute the interruption handler
  this->execute_instruction();
  // Restore the original instruction state
  this->current_instruction = saved;
  this->setFetchedData(savedData);
}

void CPU::run()
{
  this->running = true;

  try
  {
    while (this->running)
    {
      // IME takes one instruction to be enabled
      ppu->run();
      bool oldImeDisabled = this->getIME() == false;
      if (this->getIME() && interruption->hasPendingInterruption())
      {
        this->setHalt(false); // unset halt
        this->call_interruption();
      }
      serial->output_serial_data();

      // If halted, run for one cycle
      if (this->isHalted() && !interruption->hasPendingInterruption())
      {
        timer->update_timer(1);
        ppu->run();
        continue;
      }
      this->setHalt(false);      // unset halt
      this->fetch_instruction(); // fetch next instrction
#ifdef DEBUG_CPU
      debug_memory_and_registers();
#endif
      this->fetch_data();          // fetch required data from memory
      this->execute_instruction(); // process operation
      if (this->getIME() && oldImeDisabled)
      {
        this->setIME(false); // ime is enabled only on next instruction
        this->setImePC = this->getPC();
      }
      if (this->setImePC && this->getOldPC() == this->setImePC)
      {
        this->setImePC = 0;
        this->setIME(true);
      }
    }
    cout << "CPU thread finished gracefully" << endl;
  }
  catch (const std::exception &e)
  {
    std::cerr << e.what() << '\n';
  }
}

CPU *cpu = nullptr;