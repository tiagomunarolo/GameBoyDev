#pragma once

#include "cpu.hpp"

void set_register_value(CPU *cpu, Operand op, u16 value);

void update_flag_register(CPU *cpu);

u8 fetch_reg8(CPU *cpu, Operand op);

u16 fetch_reg16(CPU *cpu, Operand op);
