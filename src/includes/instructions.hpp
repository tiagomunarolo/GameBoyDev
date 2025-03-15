#pragma once

#include "definitions.hpp"
#include "mnemonics.hpp"

typedef struct _instruction_set
{
    Mnemonic mnemonic;
    u8 bytes;
    Operand op1;
    Operand op2;
    OperationType operation;
    u8 cycles;
} InstructionSet;

const char *operandToString(Operand operand);

const char *mnemonicToString(Mnemonic mnemonic);

InstructionSet get_instruction_by_opcode(u8 opcode, InstructionType type);
