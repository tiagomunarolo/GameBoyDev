#pragma once
#include <variant>

using namespace std;

enum InstructionType
{
    PREFIXED_TYPE,
    NON_PREFIXED_TYPE
};

typedef enum CpuFlags
{
    CARRY_FLAG,
    ZERO_FLAG,
    HC_FLAG,
    SUB_FLAG
} cpuFlags;

typedef enum Registers
{
    A = 0, // for cpu indexing
    B = 1,
    C = 2,
    D = 3,
    E = 4,
    F = 5,
    H = 6,
    L = 7,
    AF,
    BC,
    DE,
    HL,
    SP,
} Registers;

typedef enum operand_types
{
    A16,
    A8,
    E8,
    N16,
    N8,
    NC,
    NULL_OP,
    Z,
    NZ,
    RST_00,
    RST_08,
    RST_10,
    RST_18,
    RST_20,
    RST_28,
    RST_30,
    RST_38,
    BIT_0,
    BIT_1,
    BIT_2,
    BIT_3,
    BIT_4,
    BIT_5,
    BIT_6,
    BIT_7,
} operand_types;

typedef std::variant<operand_types, Registers> Operand;

typedef enum operation_type
{
    CONTROL_INSTRUCTION,
    IMM16_TO_R16,
    HIGHMEM_LOAD,
    IMM8_TO_R16,
    IMM8_TO_R8,
    JUMP_CALL,
    MEM_IMM16_TO_R8,
    MEM_REG16_TO_R8,
    R16_ONLY,
    R16_TO_IMM16,
    R16_to_R16,
    R8_ONLY,
    R8_TO_IMM16,
    R8_TO_MEM_R16,
    R8_TO_R8,
    PREFIXED
} OperationType;

typedef enum Mnemonic
{
    ADC,
    ADD,
    AND,
    CALL,
    CCF,
    CP,
    CPL,
    DAA,
    DEC,
    DI,
    EI,
    HALT,
    ILLEGAL_D3,
    ILLEGAL_DB,
    ILLEGAL_DD,
    ILLEGAL_E3,
    ILLEGAL_E4,
    ILLEGAL_EB,
    ILLEGAL_EC,
    ILLEGAL_ED,
    ILLEGAL_F4,
    ILLEGAL_FC,
    ILLEGAL_FD,
    INC,
    JP,
    JR,
    LD,
    LDH,
    NOP,
    OR,
    POP,
    PREFIX,
    PUSH,
    RET,
    RETI,
    RLA,
    RLCA,
    RRA,
    RRCA,
    RST,
    SBC,
    SCF,
    STOP,
    SUB,
    XOR,
    BIT,
    RES,
    RL,
    RLC,
    RR,
    RRC,
    SET,
    SLA,
    SRA,
    SRL,
    SWAP,
    INT_JOYPAD,
    INT_SERIAL,
    INT_TIMER,
    INT_LCD,
    INT_VBLANK,
    INT_UNKNOWN
} Mnemonic;