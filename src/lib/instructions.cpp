#include "instructions.hpp"

const InstructionSet instruction_set[0x100] = {
    [0x00] = {NOP, 1, NULL_OP, NULL_OP, CONTROL_INSTRUCTION, 4},
    [0x01] = {LD, 3, BC, N16, IMM16_TO_R16, 12},
    [0x02] = {LD, 1, BC, A, R8_TO_MEM_R16, 8},
    [0x03] = {INC, 1, BC, NULL_OP, R16_ONLY, 8},
    [0x04] = {INC, 1, B, NULL_OP, R8_ONLY, 4},
    [0x05] = {DEC, 1, B, NULL_OP, R8_ONLY, 4},
    [0x06] = {LD, 2, B, N8, IMM8_TO_R8, 8},
    [0x07] = {RLCA, 1, NULL_OP, NULL_OP, CONTROL_INSTRUCTION, 4},
    [0x08] = {LD, 3, A16, SP, R16_TO_IMM16, 20},
    [0x09] = {ADD, 1, HL, BC, R16_to_R16, 8},
    [0x0A] = {LD, 1, A, BC, MEM_REG16_TO_R8, 8},
    [0x0B] = {DEC, 1, BC, NULL_OP, R16_ONLY, 8},
    [0x0C] = {INC, 1, C, NULL_OP, R8_ONLY, 4},
    [0x0D] = {DEC, 1, C, NULL_OP, R8_ONLY, 4},
    [0x0E] = {LD, 2, C, N8, IMM8_TO_R8, 8},
    [0x0F] = {RRCA, 1, NULL_OP, NULL_OP, CONTROL_INSTRUCTION, 4},
    [0x10] = {STOP, 2, N8, NULL_OP, CONTROL_INSTRUCTION, 4},
    [0x11] = {LD, 3, DE, N16, IMM16_TO_R16, 12},
    [0x12] = {LD, 1, DE, A, R8_TO_MEM_R16, 8},
    [0x13] = {INC, 1, DE, NULL_OP, R16_ONLY, 8},
    [0x14] = {INC, 1, D, NULL_OP, R8_ONLY, 4},
    [0x15] = {DEC, 1, D, NULL_OP, R8_ONLY, 4},
    [0x16] = {LD, 2, D, N8, IMM8_TO_R8, 8},
    [0x17] = {RLA, 1, NULL_OP, NULL_OP, CONTROL_INSTRUCTION, 4},
    [0x18] = {JR, 2, E8, NULL_OP, JUMP_CALL, 12},
    [0x19] = {ADD, 1, HL, DE, R16_to_R16, 8},
    [0x1A] = {LD, 1, A, DE, MEM_REG16_TO_R8, 8},
    [0x1B] = {DEC, 1, DE, NULL_OP, R16_ONLY, 8},
    [0x1C] = {INC, 1, E, NULL_OP, R8_ONLY, 4},
    [0x1D] = {DEC, 1, E, NULL_OP, R8_ONLY, 4},
    [0x1E] = {LD, 2, E, N8, IMM8_TO_R8, 8},
    [0x1F] = {RRA, 1, NULL_OP, NULL_OP, CONTROL_INSTRUCTION, 4},
    [0x20] = {JR, 2, NZ, E8, JUMP_CALL, 12},
    [0x21] = {LD, 3, HL, N16, IMM16_TO_R16, 12},
    [0x22] = {LD, 1, HL, A, R8_TO_MEM_R16, 8},
    [0x23] = {INC, 1, HL, NULL_OP, R16_ONLY, 8},
    [0x24] = {INC, 1, H, NULL_OP, R8_ONLY, 4},
    [0x25] = {DEC, 1, H, NULL_OP, R8_ONLY, 4},
    [0x26] = {LD, 2, H, N8, IMM8_TO_R8, 8},
    [0x27] = {DAA, 1, NULL_OP, NULL_OP, CONTROL_INSTRUCTION, 4},
    [0x28] = {JR, 2, Z, E8, JUMP_CALL, 12},
    [0x29] = {ADD, 1, HL, HL, R16_to_R16, 8},
    [0x2A] = {LD, 1, A, HL, MEM_REG16_TO_R8, 8},
    [0x2B] = {DEC, 1, HL, NULL_OP, R16_ONLY, 8},
    [0x2C] = {INC, 1, L, NULL_OP, R8_ONLY, 4},
    [0x2D] = {DEC, 1, L, NULL_OP, R8_ONLY, 4},
    [0x2E] = {LD, 2, L, N8, IMM8_TO_R8, 8},
    [0x2F] = {CPL, 1, NULL_OP, NULL_OP, R8_ONLY, 4},
    [0x30] = {JR, 2, NC, E8, JUMP_CALL, 12},
    [0x31] = {LD, 3, SP, N16, IMM16_TO_R16, 12},
    [0x32] = {LD, 1, HL, A, R8_TO_MEM_R16, 8},
    [0x33] = {INC, 1, SP, NULL_OP, R16_ONLY, 8},
    [0x34] = {INC, 1, HL, NULL_OP, R16_ONLY, 12},
    [0x35] = {DEC, 1, HL, NULL_OP, R16_ONLY, 12},
    [0x36] = {LD, 2, HL, N8, IMM8_TO_R16, 12},
    [0x37] = {SCF, 1, NULL_OP, NULL_OP, CONTROL_INSTRUCTION, 4},
    [0x38] = {JR, 2, C, E8, JUMP_CALL, 12},
    [0x39] = {ADD, 1, HL, SP, R16_to_R16, 8},
    [0x3A] = {LD, 1, A, HL, MEM_REG16_TO_R8, 8},
    [0x3B] = {DEC, 1, SP, NULL_OP, R16_ONLY, 8},
    [0x3C] = {INC, 1, A, NULL_OP, R8_ONLY, 4},
    [0x3D] = {DEC, 1, A, NULL_OP, R8_ONLY, 4},
    [0x3E] = {LD, 2, A, N8, IMM8_TO_R8, 8},
    [0x3F] = {CCF, 1, NULL_OP, NULL_OP, CONTROL_INSTRUCTION, 4},
    [0x40] = {LD, 1, B, B, R8_TO_R8, 4},
    [0x41] = {LD, 1, B, C, R8_TO_R8, 4},
    [0x42] = {LD, 1, B, D, R8_TO_R8, 4},
    [0x43] = {LD, 1, B, E, R8_TO_R8, 4},
    [0x44] = {LD, 1, B, H, R8_TO_R8, 4},
    [0x45] = {LD, 1, B, L, R8_TO_R8, 4},
    [0x46] = {LD, 1, B, HL, MEM_REG16_TO_R8, 8},
    [0x47] = {LD, 1, B, A, R8_TO_R8, 4},
    [0x48] = {LD, 1, C, B, R8_TO_R8, 4},
    [0x49] = {LD, 1, C, C, R8_TO_R8, 4},
    [0x4A] = {LD, 1, C, D, R8_TO_R8, 4},
    [0x4B] = {LD, 1, C, E, R8_TO_R8, 4},
    [0x4C] = {LD, 1, C, H, R8_TO_R8, 4},
    [0x4D] = {LD, 1, C, L, R8_TO_R8, 4},
    [0x4E] = {LD, 1, C, HL, MEM_REG16_TO_R8, 8},
    [0x4F] = {LD, 1, C, A, R8_TO_R8, 4},
    [0x50] = {LD, 1, D, B, R8_TO_R8, 4},
    [0x51] = {LD, 1, D, C, R8_TO_R8, 4},
    [0x52] = {LD, 1, D, D, R8_TO_R8, 4},
    [0x53] = {LD, 1, D, E, R8_TO_R8, 4},
    [0x54] = {LD, 1, D, H, R8_TO_R8, 4},
    [0x55] = {LD, 1, D, L, R8_TO_R8, 4},
    [0x56] = {LD, 1, D, HL, MEM_REG16_TO_R8, 8},
    [0x57] = {LD, 1, D, A, R8_TO_R8, 4},
    [0x58] = {LD, 1, E, B, R8_TO_R8, 4},
    [0x59] = {LD, 1, E, C, R8_TO_R8, 4},
    [0x5A] = {LD, 1, E, D, R8_TO_R8, 4},
    [0x5B] = {LD, 1, E, E, R8_TO_R8, 4},
    [0x5C] = {LD, 1, E, H, R8_TO_R8, 4},
    [0x5D] = {LD, 1, E, L, R8_TO_R8, 4},
    [0x5E] = {LD, 1, E, HL, MEM_REG16_TO_R8, 8},
    [0x5F] = {LD, 1, E, A, R8_TO_R8, 4},
    [0x60] = {LD, 1, H, B, R8_TO_R8, 4},
    [0x61] = {LD, 1, H, C, R8_TO_R8, 4},
    [0x62] = {LD, 1, H, D, R8_TO_R8, 4},
    [0x63] = {LD, 1, H, E, R8_TO_R8, 4},
    [0x64] = {LD, 1, H, H, R8_TO_R8, 4},
    [0x65] = {LD, 1, H, L, R8_TO_R8, 4},
    [0x66] = {LD, 1, H, HL, MEM_REG16_TO_R8, 8},
    [0x67] = {LD, 1, H, A, R8_TO_R8, 4},
    [0x68] = {LD, 1, L, B, R8_TO_R8, 4},
    [0x69] = {LD, 1, L, C, R8_TO_R8, 4},
    [0x6A] = {LD, 1, L, D, R8_TO_R8, 4},
    [0x6B] = {LD, 1, L, E, R8_TO_R8, 4},
    [0x6C] = {LD, 1, L, H, R8_TO_R8, 4},
    [0x6D] = {LD, 1, L, L, R8_TO_R8, 4},
    [0x6E] = {LD, 1, L, HL, MEM_REG16_TO_R8, 8},
    [0x6F] = {LD, 1, L, A, R8_TO_R8, 4},
    [0x70] = {LD, 1, HL, B, R8_TO_MEM_R16, 8},
    [0x71] = {LD, 1, HL, C, R8_TO_MEM_R16, 8},
    [0x72] = {LD, 1, HL, D, R8_TO_MEM_R16, 8},
    [0x73] = {LD, 1, HL, E, R8_TO_MEM_R16, 8},
    [0x74] = {LD, 1, HL, H, R8_TO_MEM_R16, 8},
    [0x75] = {LD, 1, HL, L, R8_TO_MEM_R16, 8},
    [0x76] = {HALT, 1, NULL_OP, NULL_OP, CONTROL_INSTRUCTION, 4},
    [0x77] = {LD, 1, HL, A, R8_TO_MEM_R16, 8},
    [0x78] = {LD, 1, A, B, R8_TO_R8, 4},
    [0x79] = {LD, 1, A, C, R8_TO_R8, 4},
    [0x7A] = {LD, 1, A, D, R8_TO_R8, 4},
    [0x7B] = {LD, 1, A, E, R8_TO_R8, 4},
    [0x7C] = {LD, 1, A, H, R8_TO_R8, 4},
    [0x7D] = {LD, 1, A, L, R8_TO_R8, 4},
    [0x7E] = {LD, 1, A, HL, MEM_REG16_TO_R8, 8},
    [0x7F] = {LD, 1, A, A, R8_TO_R8, 4},
    [0x80] = {ADD, 1, A, B, R8_TO_R8, 4},
    [0x81] = {ADD, 1, A, C, R8_TO_R8, 4},
    [0x82] = {ADD, 1, A, D, R8_TO_R8, 4},
    [0x83] = {ADD, 1, A, E, R8_TO_R8, 4},
    [0x84] = {ADD, 1, A, H, R8_TO_R8, 4},
    [0x85] = {ADD, 1, A, L, R8_TO_R8, 4},
    [0x86] = {ADD, 1, A, HL, MEM_REG16_TO_R8, 8},
    [0x87] = {ADD, 1, A, A, R8_TO_R8, 4},
    [0x88] = {ADC, 1, A, B, R8_TO_R8, 4},
    [0x89] = {ADC, 1, A, C, R8_TO_R8, 4},
    [0x8A] = {ADC, 1, A, D, R8_TO_R8, 4},
    [0x8B] = {ADC, 1, A, E, R8_TO_R8, 4},
    [0x8C] = {ADC, 1, A, H, R8_TO_R8, 4},
    [0x8D] = {ADC, 1, A, L, R8_TO_R8, 4},
    [0x8E] = {ADC, 1, A, HL, MEM_REG16_TO_R8, 8},
    [0x8F] = {ADC, 1, A, A, R8_TO_R8, 4},
    [0x90] = {SUB, 1, A, B, R8_TO_R8, 4},
    [0x91] = {SUB, 1, A, C, R8_TO_R8, 4},
    [0x92] = {SUB, 1, A, D, R8_TO_R8, 4},
    [0x93] = {SUB, 1, A, E, R8_TO_R8, 4},
    [0x94] = {SUB, 1, A, H, R8_TO_R8, 4},
    [0x95] = {SUB, 1, A, L, R8_TO_R8, 4},
    [0x96] = {SUB, 1, A, HL, MEM_REG16_TO_R8, 8},
    [0x97] = {SUB, 1, A, A, R8_TO_R8, 4},
    [0x98] = {SBC, 1, A, B, R8_TO_R8, 4},
    [0x99] = {SBC, 1, A, C, R8_TO_R8, 4},
    [0x9A] = {SBC, 1, A, D, R8_TO_R8, 4},
    [0x9B] = {SBC, 1, A, E, R8_TO_R8, 4},
    [0x9C] = {SBC, 1, A, H, R8_TO_R8, 4},
    [0x9D] = {SBC, 1, A, L, R8_TO_R8, 4},
    [0x9E] = {SBC, 1, A, HL, MEM_REG16_TO_R8, 8},
    [0x9F] = {SBC, 1, A, A, R8_TO_R8, 4},
    [0xA0] = {AND, 1, A, B, R8_TO_R8, 4},
    [0xA1] = {AND, 1, A, C, R8_TO_R8, 4},
    [0xA2] = {AND, 1, A, D, R8_TO_R8, 4},
    [0xA3] = {AND, 1, A, E, R8_TO_R8, 4},
    [0xA4] = {AND, 1, A, H, R8_TO_R8, 4},
    [0xA5] = {AND, 1, A, L, R8_TO_R8, 4},
    [0xA6] = {AND, 1, A, HL, MEM_REG16_TO_R8, 8},
    [0xA7] = {AND, 1, A, A, R8_TO_R8, 4},
    [0xA8] = {XOR, 1, A, B, R8_TO_R8, 4},
    [0xA9] = {XOR, 1, A, C, R8_TO_R8, 4},
    [0xAA] = {XOR, 1, A, D, R8_TO_R8, 4},
    [0xAB] = {XOR, 1, A, E, R8_TO_R8, 4},
    [0xAC] = {XOR, 1, A, H, R8_TO_R8, 4},
    [0xAD] = {XOR, 1, A, L, R8_TO_R8, 4},
    [0xAE] = {XOR, 1, A, HL, MEM_REG16_TO_R8, 8},
    [0xAF] = {XOR, 1, A, A, R8_TO_R8, 4},
    [0xB0] = {OR, 1, A, B, R8_TO_R8, 4},
    [0xB1] = {OR, 1, A, C, R8_TO_R8, 4},
    [0xB2] = {OR, 1, A, D, R8_TO_R8, 4},
    [0xB3] = {OR, 1, A, E, R8_TO_R8, 4},
    [0xB4] = {OR, 1, A, H, R8_TO_R8, 4},
    [0xB5] = {OR, 1, A, L, R8_TO_R8, 4},
    [0xB6] = {OR, 1, A, HL, MEM_REG16_TO_R8, 8},
    [0xB7] = {OR, 1, A, A, R8_TO_R8, 4},
    [0xB8] = {CP, 1, A, B, R8_TO_R8, 4},
    [0xB9] = {CP, 1, A, C, R8_TO_R8, 4},
    [0xBA] = {CP, 1, A, D, R8_TO_R8, 4},
    [0xBB] = {CP, 1, A, E, R8_TO_R8, 4},
    [0xBC] = {CP, 1, A, H, R8_TO_R8, 4},
    [0xBD] = {CP, 1, A, L, R8_TO_R8, 4},
    [0xBE] = {CP, 1, A, HL, MEM_REG16_TO_R8, 8},
    [0xBF] = {CP, 1, A, A, R8_TO_R8, 4},
    [0xC0] = {RET, 1, NZ, NULL_OP, CONTROL_INSTRUCTION, 20},
    [0xC1] = {POP, 1, BC, NULL_OP, R16_ONLY, 12},
    [0xC2] = {JP, 3, NZ, A16, JUMP_CALL, 16},
    [0xC3] = {JP, 3, A16, NULL_OP, JUMP_CALL, 16},
    [0xC4] = {CALL, 3, NZ, A16, JUMP_CALL, 24},
    [0xC5] = {PUSH, 1, BC, NULL_OP, R16_ONLY, 16},
    [0xC6] = {ADD, 2, A, N8, IMM8_TO_R8, 8},
    [0xC7] = {RST, 1, RST_00, NULL_OP, JUMP_CALL, 16},
    [0xC8] = {RET, 1, Z, NULL_OP, CONTROL_INSTRUCTION, 20},
    [0xC9] = {RET, 1, NULL_OP, NULL_OP, CONTROL_INSTRUCTION, 16},
    [0xCA] = {JP, 3, Z, A16, JUMP_CALL, 16},
    [0xCB] = {PREFIX, 1, NULL_OP, NULL_OP, CONTROL_INSTRUCTION, 4},
    [0xCC] = {CALL, 3, Z, A16, JUMP_CALL, 24},
    [0xCD] = {CALL, 3, A16, NULL_OP, JUMP_CALL, 24},
    [0xCE] = {ADC, 2, A, N8, IMM8_TO_R8, 8},
    [0xCF] = {RST, 1, RST_08, NULL_OP, JUMP_CALL, 16},
    [0xD0] = {RET, 1, NC, NULL_OP, CONTROL_INSTRUCTION, 20},
    [0xD1] = {POP, 1, DE, NULL_OP, R16_ONLY, 12},
    [0xD2] = {JP, 3, NC, A16, JUMP_CALL, 16},
    [0xD3] = {ILLEGAL_D3, 1, NULL_OP, NULL_OP, CONTROL_INSTRUCTION, 4},
    [0xD4] = {CALL, 3, NC, A16, JUMP_CALL, 24},
    [0xD5] = {PUSH, 1, DE, NULL_OP, R16_ONLY, 16},
    [0xD6] = {SUB, 2, A, N8, IMM8_TO_R8, 8},
    [0xD7] = {RST, 1, RST_10, NULL_OP, JUMP_CALL, 16},
    [0xD8] = {RET, 1, C, NULL_OP, CONTROL_INSTRUCTION, 20},
    [0xD9] = {RETI, 1, NULL_OP, NULL_OP, CONTROL_INSTRUCTION, 16},
    [0xDA] = {JP, 3, C, A16, JUMP_CALL, 16},
    [0xDB] = {ILLEGAL_DB, 1, NULL_OP, NULL_OP, CONTROL_INSTRUCTION, 4},
    [0xDC] = {CALL, 3, C, A16, JUMP_CALL, 24},
    [0xDD] = {ILLEGAL_DD, 1, NULL_OP, NULL_OP, CONTROL_INSTRUCTION, 4},
    [0xDE] = {SBC, 2, A, N8, IMM8_TO_R8, 8},
    [0xDF] = {RST, 1, RST_18, NULL_OP, JUMP_CALL, 16},
    [0xE0] = {LDH, 2, A8, A, HIGHMEM_LOAD, 12},
    [0xE1] = {POP, 1, HL, NULL_OP, R16_ONLY, 12},
    [0xE2] = {LDH, 1, C, A, HIGHMEM_LOAD, 8},
    [0xE3] = {ILLEGAL_E3, 1, NULL_OP, NULL_OP, CONTROL_INSTRUCTION, 4},
    [0xE4] = {ILLEGAL_E4, 1, NULL_OP, NULL_OP, CONTROL_INSTRUCTION, 4},
    [0xE5] = {PUSH, 1, HL, NULL_OP, R16_ONLY, 16},
    [0xE6] = {AND, 2, A, N8, IMM8_TO_R8, 8},
    [0xE7] = {RST, 1, RST_20, NULL_OP, JUMP_CALL, 16},
    [0xE8] = {ADD, 2, SP, E8, IMM8_TO_R16, 16},
    [0xE9] = {JP, 1, HL, NULL_OP, R16_ONLY, 4},
    [0xEA] = {LD, 3, A16, A, R8_TO_IMM16, 16},
    [0xEB] = {ILLEGAL_EB, 1, NULL_OP, NULL_OP, CONTROL_INSTRUCTION, 4},
    [0xEC] = {ILLEGAL_EC, 1, NULL_OP, NULL_OP, CONTROL_INSTRUCTION, 4},
    [0xED] = {ILLEGAL_ED, 1, NULL_OP, NULL_OP, CONTROL_INSTRUCTION, 4},
    [0xEE] = {XOR, 2, A, N8, IMM8_TO_R8, 8},
    [0xEF] = {RST, 1, RST_28, NULL_OP, JUMP_CALL, 16},
    [0xF0] = {LDH, 2, A, A8, HIGHMEM_LOAD, 12},
    [0xF1] = {POP, 1, AF, NULL_OP, R16_ONLY, 12},
    [0xF2] = {LDH, 1, A, C, HIGHMEM_LOAD, 8},
    [0xF3] = {DI, 1, NULL_OP, NULL_OP, CONTROL_INSTRUCTION, 4},
    [0xF4] = {ILLEGAL_F4, 1, NULL_OP, NULL_OP, CONTROL_INSTRUCTION, 4},
    [0xF5] = {PUSH, 1, AF, NULL_OP, R16_ONLY, 16},
    [0xF6] = {OR, 2, A, N8, IMM8_TO_R8, 8},
    [0xF7] = {RST, 1, RST_30, NULL_OP, JUMP_CALL, 16},
    [0xF8] = {LD, 2, HL, SP, R16_to_R16, 12},
    [0xF9] = {LD, 1, SP, HL, R16_to_R16, 8},
    [0xFA] = {LD, 3, A, A16, MEM_IMM16_TO_R8, 16},
    [0xFB] = {EI, 1, NULL_OP, NULL_OP, CONTROL_INSTRUCTION, 4},
    [0xFC] = {ILLEGAL_FC, 1, NULL_OP, NULL_OP, CONTROL_INSTRUCTION, 4},
    [0xFD] = {ILLEGAL_FD, 1, NULL_OP, NULL_OP, CONTROL_INSTRUCTION, 4},
    [0xFE] = {CP, 2, A, N8, IMM8_TO_R8, 8},
    [0xFF] = {RST, 1, RST_38, NULL_OP, JUMP_CALL, 16},
};

const InstructionSet instruction_set_prefixed[0x100] = {
    [0x00] = {RLC, 2, B, NULL_OP, PREFIXED, 8},
    [0x01] = {RLC, 2, C, NULL_OP, PREFIXED, 8},
    [0x02] = {RLC, 2, D, NULL_OP, PREFIXED, 8},
    [0x03] = {RLC, 2, E, NULL_OP, PREFIXED, 8},
    [0x04] = {RLC, 2, H, NULL_OP, PREFIXED, 8},
    [0x05] = {RLC, 2, L, NULL_OP, PREFIXED, 8},
    [0x06] = {RLC, 2, HL, NULL_OP, PREFIXED, 16},
    [0x07] = {RLC, 2, A, NULL_OP, PREFIXED, 8},
    [0x08] = {RRC, 2, B, NULL_OP, PREFIXED, 8},
    [0x09] = {RRC, 2, C, NULL_OP, PREFIXED, 8},
    [0x0A] = {RRC, 2, D, NULL_OP, PREFIXED, 8},
    [0x0B] = {RRC, 2, E, NULL_OP, PREFIXED, 8},
    [0x0C] = {RRC, 2, H, NULL_OP, PREFIXED, 8},
    [0x0D] = {RRC, 2, L, NULL_OP, PREFIXED, 8},
    [0x0E] = {RRC, 2, HL, NULL_OP, PREFIXED, 16},
    [0x0F] = {RRC, 2, A, NULL_OP, PREFIXED, 8},
    [0x10] = {RL, 2, B, NULL_OP, PREFIXED, 8},
    [0x11] = {RL, 2, C, NULL_OP, PREFIXED, 8},
    [0x12] = {RL, 2, D, NULL_OP, PREFIXED, 8},
    [0x13] = {RL, 2, E, NULL_OP, PREFIXED, 8},
    [0x14] = {RL, 2, H, NULL_OP, PREFIXED, 8},
    [0x15] = {RL, 2, L, NULL_OP, PREFIXED, 8},
    [0x16] = {RL, 2, HL, NULL_OP, PREFIXED, 16},
    [0x17] = {RL, 2, A, NULL_OP, PREFIXED, 8},
    [0x18] = {RR, 2, B, NULL_OP, PREFIXED, 8},
    [0x19] = {RR, 2, C, NULL_OP, PREFIXED, 8},
    [0x1A] = {RR, 2, D, NULL_OP, PREFIXED, 8},
    [0x1B] = {RR, 2, E, NULL_OP, PREFIXED, 8},
    [0x1C] = {RR, 2, H, NULL_OP, PREFIXED, 8},
    [0x1D] = {RR, 2, L, NULL_OP, PREFIXED, 8},
    [0x1E] = {RR, 2, HL, NULL_OP, PREFIXED, 16},
    [0x1F] = {RR, 2, A, NULL_OP, PREFIXED, 8},
    [0x20] = {SLA, 2, B, NULL_OP, PREFIXED, 8},
    [0x21] = {SLA, 2, C, NULL_OP, PREFIXED, 8},
    [0x22] = {SLA, 2, D, NULL_OP, PREFIXED, 8},
    [0x23] = {SLA, 2, E, NULL_OP, PREFIXED, 8},
    [0x24] = {SLA, 2, H, NULL_OP, PREFIXED, 8},
    [0x25] = {SLA, 2, L, NULL_OP, PREFIXED, 8},
    [0x26] = {SLA, 2, HL, NULL_OP, PREFIXED, 16},
    [0x27] = {SLA, 2, A, NULL_OP, PREFIXED, 8},
    [0x28] = {SRA, 2, B, NULL_OP, PREFIXED, 8},
    [0x29] = {SRA, 2, C, NULL_OP, PREFIXED, 8},
    [0x2A] = {SRA, 2, D, NULL_OP, PREFIXED, 8},
    [0x2B] = {SRA, 2, E, NULL_OP, PREFIXED, 8},
    [0x2C] = {SRA, 2, H, NULL_OP, PREFIXED, 8},
    [0x2D] = {SRA, 2, L, NULL_OP, PREFIXED, 8},
    [0x2E] = {SRA, 2, HL, NULL_OP, PREFIXED, 16},
    [0x2F] = {SRA, 2, A, NULL_OP, PREFIXED, 8},
    [0x30] = {SWAP, 2, B, NULL_OP, PREFIXED, 8},
    [0x31] = {SWAP, 2, C, NULL_OP, PREFIXED, 8},
    [0x32] = {SWAP, 2, D, NULL_OP, PREFIXED, 8},
    [0x33] = {SWAP, 2, E, NULL_OP, PREFIXED, 8},
    [0x34] = {SWAP, 2, H, NULL_OP, PREFIXED, 8},
    [0x35] = {SWAP, 2, L, NULL_OP, PREFIXED, 8},
    [0x36] = {SWAP, 2, HL, NULL_OP, PREFIXED, 16},
    [0x37] = {SWAP, 2, A, NULL_OP, PREFIXED, 8},
    [0x38] = {SRL, 2, B, NULL_OP, PREFIXED, 8},
    [0x39] = {SRL, 2, C, NULL_OP, PREFIXED, 8},
    [0x3A] = {SRL, 2, D, NULL_OP, PREFIXED, 8},
    [0x3B] = {SRL, 2, E, NULL_OP, PREFIXED, 8},
    [0x3C] = {SRL, 2, H, NULL_OP, PREFIXED, 8},
    [0x3D] = {SRL, 2, L, NULL_OP, PREFIXED, 8},
    [0x3E] = {SRL, 2, HL, NULL_OP, PREFIXED, 16},
    [0x3F] = {SRL, 2, A, NULL_OP, PREFIXED, 8},
    [0x40] = {BIT, 2, BIT_0, B, PREFIXED, 8},
    [0x41] = {BIT, 2, BIT_0, C, PREFIXED, 8},
    [0x42] = {BIT, 2, BIT_0, D, PREFIXED, 8},
    [0x43] = {BIT, 2, BIT_0, E, PREFIXED, 8},
    [0x44] = {BIT, 2, BIT_0, H, PREFIXED, 8},
    [0x45] = {BIT, 2, BIT_0, L, PREFIXED, 8},
    [0x46] = {BIT, 2, BIT_0, HL, PREFIXED, 12},
    [0x47] = {BIT, 2, BIT_0, A, PREFIXED, 8},
    [0x48] = {BIT, 2, BIT_1, B, PREFIXED, 8},
    [0x49] = {BIT, 2, BIT_1, C, PREFIXED, 8},
    [0x4A] = {BIT, 2, BIT_1, D, PREFIXED, 8},
    [0x4B] = {BIT, 2, BIT_1, E, PREFIXED, 8},
    [0x4C] = {BIT, 2, BIT_1, H, PREFIXED, 8},
    [0x4D] = {BIT, 2, BIT_1, L, PREFIXED, 8},
    [0x4E] = {BIT, 2, BIT_1, HL, PREFIXED, 12},
    [0x4F] = {BIT, 2, BIT_1, A, PREFIXED, 8},
    [0x50] = {BIT, 2, BIT_2, B, PREFIXED, 8},
    [0x51] = {BIT, 2, BIT_2, C, PREFIXED, 8},
    [0x52] = {BIT, 2, BIT_2, D, PREFIXED, 8},
    [0x53] = {BIT, 2, BIT_2, E, PREFIXED, 8},
    [0x54] = {BIT, 2, BIT_2, H, PREFIXED, 8},
    [0x55] = {BIT, 2, BIT_2, L, PREFIXED, 8},
    [0x56] = {BIT, 2, BIT_2, HL, PREFIXED, 12},
    [0x57] = {BIT, 2, BIT_2, A, PREFIXED, 8},
    [0x58] = {BIT, 2, BIT_3, B, PREFIXED, 8},
    [0x59] = {BIT, 2, BIT_3, C, PREFIXED, 8},
    [0x5A] = {BIT, 2, BIT_3, D, PREFIXED, 8},
    [0x5B] = {BIT, 2, BIT_3, E, PREFIXED, 8},
    [0x5C] = {BIT, 2, BIT_3, H, PREFIXED, 8},
    [0x5D] = {BIT, 2, BIT_3, L, PREFIXED, 8},
    [0x5E] = {BIT, 2, BIT_3, HL, PREFIXED, 12},
    [0x5F] = {BIT, 2, BIT_3, A, PREFIXED, 8},
    [0x60] = {BIT, 2, BIT_4, B, PREFIXED, 8},
    [0x61] = {BIT, 2, BIT_4, C, PREFIXED, 8},
    [0x62] = {BIT, 2, BIT_4, D, PREFIXED, 8},
    [0x63] = {BIT, 2, BIT_4, E, PREFIXED, 8},
    [0x64] = {BIT, 2, BIT_4, H, PREFIXED, 8},
    [0x65] = {BIT, 2, BIT_4, L, PREFIXED, 8},
    [0x66] = {BIT, 2, BIT_4, HL, PREFIXED, 12},
    [0x67] = {BIT, 2, BIT_4, A, PREFIXED, 8},
    [0x68] = {BIT, 2, BIT_5, B, PREFIXED, 8},
    [0x69] = {BIT, 2, BIT_5, C, PREFIXED, 8},
    [0x6A] = {BIT, 2, BIT_5, D, PREFIXED, 8},
    [0x6B] = {BIT, 2, BIT_5, E, PREFIXED, 8},
    [0x6C] = {BIT, 2, BIT_5, H, PREFIXED, 8},
    [0x6D] = {BIT, 2, BIT_5, L, PREFIXED, 8},
    [0x6E] = {BIT, 2, BIT_5, HL, PREFIXED, 12},
    [0x6F] = {BIT, 2, BIT_5, A, PREFIXED, 8},
    [0x70] = {BIT, 2, BIT_6, B, PREFIXED, 8},
    [0x71] = {BIT, 2, BIT_6, C, PREFIXED, 8},
    [0x72] = {BIT, 2, BIT_6, D, PREFIXED, 8},
    [0x73] = {BIT, 2, BIT_6, E, PREFIXED, 8},
    [0x74] = {BIT, 2, BIT_6, H, PREFIXED, 8},
    [0x75] = {BIT, 2, BIT_6, L, PREFIXED, 8},
    [0x76] = {BIT, 2, BIT_6, HL, PREFIXED, 12},
    [0x77] = {BIT, 2, BIT_6, A, PREFIXED, 8},
    [0x78] = {BIT, 2, BIT_7, B, PREFIXED, 8},
    [0x79] = {BIT, 2, BIT_7, C, PREFIXED, 8},
    [0x7A] = {BIT, 2, BIT_7, D, PREFIXED, 8},
    [0x7B] = {BIT, 2, BIT_7, E, PREFIXED, 8},
    [0x7C] = {BIT, 2, BIT_7, H, PREFIXED, 8},
    [0x7D] = {BIT, 2, BIT_7, L, PREFIXED, 8},
    [0x7E] = {BIT, 2, BIT_7, HL, PREFIXED, 12},
    [0x7F] = {BIT, 2, BIT_7, A, PREFIXED, 8},
    [0x80] = {RES, 2, BIT_0, B, PREFIXED, 8},
    [0x81] = {RES, 2, BIT_0, C, PREFIXED, 8},
    [0x82] = {RES, 2, BIT_0, D, PREFIXED, 8},
    [0x83] = {RES, 2, BIT_0, E, PREFIXED, 8},
    [0x84] = {RES, 2, BIT_0, H, PREFIXED, 8},
    [0x85] = {RES, 2, BIT_0, L, PREFIXED, 8},
    [0x86] = {RES, 2, BIT_0, HL, PREFIXED, 16},
    [0x87] = {RES, 2, BIT_0, A, PREFIXED, 8},
    [0x88] = {RES, 2, BIT_1, B, PREFIXED, 8},
    [0x89] = {RES, 2, BIT_1, C, PREFIXED, 8},
    [0x8A] = {RES, 2, BIT_1, D, PREFIXED, 8},
    [0x8B] = {RES, 2, BIT_1, E, PREFIXED, 8},
    [0x8C] = {RES, 2, BIT_1, H, PREFIXED, 8},
    [0x8D] = {RES, 2, BIT_1, L, PREFIXED, 8},
    [0x8E] = {RES, 2, BIT_1, HL, PREFIXED, 16},
    [0x8F] = {RES, 2, BIT_1, A, PREFIXED, 8},
    [0x90] = {RES, 2, BIT_2, B, PREFIXED, 8},
    [0x91] = {RES, 2, BIT_2, C, PREFIXED, 8},
    [0x92] = {RES, 2, BIT_2, D, PREFIXED, 8},
    [0x93] = {RES, 2, BIT_2, E, PREFIXED, 8},
    [0x94] = {RES, 2, BIT_2, H, PREFIXED, 8},
    [0x95] = {RES, 2, BIT_2, L, PREFIXED, 8},
    [0x96] = {RES, 2, BIT_2, HL, PREFIXED, 16},
    [0x97] = {RES, 2, BIT_2, A, PREFIXED, 8},
    [0x98] = {RES, 2, BIT_3, B, PREFIXED, 8},
    [0x99] = {RES, 2, BIT_3, C, PREFIXED, 8},
    [0x9A] = {RES, 2, BIT_3, D, PREFIXED, 8},
    [0x9B] = {RES, 2, BIT_3, E, PREFIXED, 8},
    [0x9C] = {RES, 2, BIT_3, H, PREFIXED, 8},
    [0x9D] = {RES, 2, BIT_3, L, PREFIXED, 8},
    [0x9E] = {RES, 2, BIT_3, HL, PREFIXED, 16},
    [0x9F] = {RES, 2, BIT_3, A, PREFIXED, 8},
    [0xA0] = {RES, 2, BIT_4, B, PREFIXED, 8},
    [0xA1] = {RES, 2, BIT_4, C, PREFIXED, 8},
    [0xA2] = {RES, 2, BIT_4, D, PREFIXED, 8},
    [0xA3] = {RES, 2, BIT_4, E, PREFIXED, 8},
    [0xA4] = {RES, 2, BIT_4, H, PREFIXED, 8},
    [0xA5] = {RES, 2, BIT_4, L, PREFIXED, 8},
    [0xA6] = {RES, 2, BIT_4, HL, PREFIXED, 16},
    [0xA7] = {RES, 2, BIT_4, A, PREFIXED, 8},
    [0xA8] = {RES, 2, BIT_5, B, PREFIXED, 8},
    [0xA9] = {RES, 2, BIT_5, C, PREFIXED, 8},
    [0xAA] = {RES, 2, BIT_5, D, PREFIXED, 8},
    [0xAB] = {RES, 2, BIT_5, E, PREFIXED, 8},
    [0xAC] = {RES, 2, BIT_5, H, PREFIXED, 8},
    [0xAD] = {RES, 2, BIT_5, L, PREFIXED, 8},
    [0xAE] = {RES, 2, BIT_5, HL, PREFIXED, 16},
    [0xAF] = {RES, 2, BIT_5, A, PREFIXED, 8},
    [0xB0] = {RES, 2, BIT_6, B, PREFIXED, 8},
    [0xB1] = {RES, 2, BIT_6, C, PREFIXED, 8},
    [0xB2] = {RES, 2, BIT_6, D, PREFIXED, 8},
    [0xB3] = {RES, 2, BIT_6, E, PREFIXED, 8},
    [0xB4] = {RES, 2, BIT_6, H, PREFIXED, 8},
    [0xB5] = {RES, 2, BIT_6, L, PREFIXED, 8},
    [0xB6] = {RES, 2, BIT_6, HL, PREFIXED, 16},
    [0xB7] = {RES, 2, BIT_6, A, PREFIXED, 8},
    [0xB8] = {RES, 2, BIT_7, B, PREFIXED, 8},
    [0xB9] = {RES, 2, BIT_7, C, PREFIXED, 8},
    [0xBA] = {RES, 2, BIT_7, D, PREFIXED, 8},
    [0xBB] = {RES, 2, BIT_7, E, PREFIXED, 8},
    [0xBC] = {RES, 2, BIT_7, H, PREFIXED, 8},
    [0xBD] = {RES, 2, BIT_7, L, PREFIXED, 8},
    [0xBE] = {RES, 2, BIT_7, HL, PREFIXED, 16},
    [0xBF] = {RES, 2, BIT_7, A, PREFIXED, 8},
    [0xC0] = {SET, 2, BIT_0, B, PREFIXED, 8},
    [0xC1] = {SET, 2, BIT_0, C, PREFIXED, 8},
    [0xC2] = {SET, 2, BIT_0, D, PREFIXED, 8},
    [0xC3] = {SET, 2, BIT_0, E, PREFIXED, 8},
    [0xC4] = {SET, 2, BIT_0, H, PREFIXED, 8},
    [0xC5] = {SET, 2, BIT_0, L, PREFIXED, 8},
    [0xC6] = {SET, 2, BIT_0, HL, PREFIXED, 16},
    [0xC7] = {SET, 2, BIT_0, A, PREFIXED, 8},
    [0xC8] = {SET, 2, BIT_1, B, PREFIXED, 8},
    [0xC9] = {SET, 2, BIT_1, C, PREFIXED, 8},
    [0xCA] = {SET, 2, BIT_1, D, PREFIXED, 8},
    [0xCB] = {SET, 2, BIT_1, E, PREFIXED, 8},
    [0xCC] = {SET, 2, BIT_1, H, PREFIXED, 8},
    [0xCD] = {SET, 2, BIT_1, L, PREFIXED, 8},
    [0xCE] = {SET, 2, BIT_1, HL, PREFIXED, 16},
    [0xCF] = {SET, 2, BIT_1, A, PREFIXED, 8},
    [0xD0] = {SET, 2, BIT_2, B, PREFIXED, 8},
    [0xD1] = {SET, 2, BIT_2, C, PREFIXED, 8},
    [0xD2] = {SET, 2, BIT_2, D, PREFIXED, 8},
    [0xD3] = {SET, 2, BIT_2, E, PREFIXED, 8},
    [0xD4] = {SET, 2, BIT_2, H, PREFIXED, 8},
    [0xD5] = {SET, 2, BIT_2, L, PREFIXED, 8},
    [0xD6] = {SET, 2, BIT_2, HL, PREFIXED, 16},
    [0xD7] = {SET, 2, BIT_2, A, PREFIXED, 8},
    [0xD8] = {SET, 2, BIT_3, B, PREFIXED, 8},
    [0xD9] = {SET, 2, BIT_3, C, PREFIXED, 8},
    [0xDA] = {SET, 2, BIT_3, D, PREFIXED, 8},
    [0xDB] = {SET, 2, BIT_3, E, PREFIXED, 8},
    [0xDC] = {SET, 2, BIT_3, H, PREFIXED, 8},
    [0xDD] = {SET, 2, BIT_3, L, PREFIXED, 8},
    [0xDE] = {SET, 2, BIT_3, HL, PREFIXED, 16},
    [0xDF] = {SET, 2, BIT_3, A, PREFIXED, 8},
    [0xE0] = {SET, 2, BIT_4, B, PREFIXED, 8},
    [0xE1] = {SET, 2, BIT_4, C, PREFIXED, 8},
    [0xE2] = {SET, 2, BIT_4, D, PREFIXED, 8},
    [0xE3] = {SET, 2, BIT_4, E, PREFIXED, 8},
    [0xE4] = {SET, 2, BIT_4, H, PREFIXED, 8},
    [0xE5] = {SET, 2, BIT_4, L, PREFIXED, 8},
    [0xE6] = {SET, 2, BIT_4, HL, PREFIXED, 16},
    [0xE7] = {SET, 2, BIT_4, A, PREFIXED, 8},
    [0xE8] = {SET, 2, BIT_5, B, PREFIXED, 8},
    [0xE9] = {SET, 2, BIT_5, C, PREFIXED, 8},
    [0xEA] = {SET, 2, BIT_5, D, PREFIXED, 8},
    [0xEB] = {SET, 2, BIT_5, E, PREFIXED, 8},
    [0xEC] = {SET, 2, BIT_5, H, PREFIXED, 8},
    [0xED] = {SET, 2, BIT_5, L, PREFIXED, 8},
    [0xEE] = {SET, 2, BIT_5, HL, PREFIXED, 16},
    [0xEF] = {SET, 2, BIT_5, A, PREFIXED, 8},
    [0xF0] = {SET, 2, BIT_6, B, PREFIXED, 8},
    [0xF1] = {SET, 2, BIT_6, C, PREFIXED, 8},
    [0xF2] = {SET, 2, BIT_6, D, PREFIXED, 8},
    [0xF3] = {SET, 2, BIT_6, E, PREFIXED, 8},
    [0xF4] = {SET, 2, BIT_6, H, PREFIXED, 8},
    [0xF5] = {SET, 2, BIT_6, L, PREFIXED, 8},
    [0xF6] = {SET, 2, BIT_6, HL, PREFIXED, 16},
    [0xF7] = {SET, 2, BIT_6, A, PREFIXED, 8},
    [0xF8] = {SET, 2, BIT_7, B, PREFIXED, 8},
    [0xF9] = {SET, 2, BIT_7, C, PREFIXED, 8},
    [0xFA] = {SET, 2, BIT_7, D, PREFIXED, 8},
    [0xFB] = {SET, 2, BIT_7, E, PREFIXED, 8},
    [0xFC] = {SET, 2, BIT_7, H, PREFIXED, 8},
    [0xFD] = {SET, 2, BIT_7, L, PREFIXED, 8},
    [0xFE] = {SET, 2, BIT_7, HL, PREFIXED, 16},
    [0xFF] = {SET, 2, BIT_7, A, PREFIXED, 8},
};

const char *mnemonicToString(Mnemonic mnemonic)
{
    switch (mnemonic)
    {
    case ADC:
        return "ADC";
    case ADD:
        return "ADD";
    case AND:
        return "AND";
    case CALL:
        return "CALL";
    case CCF:
        return "CCF";
    case CP:
        return "CP";
    case CPL:
        return "CPL";
    case DAA:
        return "DAA";
    case DEC:
        return "DEC";
    case DI:
        return "DI";
    case EI:
        return "EI";
    case HALT:
        return "HALT";
    case ILLEGAL_D3:
        return "ILLEGAL_D3";
    case ILLEGAL_DB:
        return "ILLEGAL_DB";
    case ILLEGAL_DD:
        return "ILLEGAL_DD";
    case ILLEGAL_E3:
        return "ILLEGAL_E3";
    case ILLEGAL_E4:
        return "ILLEGAL_E4";
    case ILLEGAL_EB:
        return "ILLEGAL_EB";
    case ILLEGAL_EC:
        return "ILLEGAL_EC";
    case ILLEGAL_ED:
        return "ILLEGAL_ED";
    case ILLEGAL_F4:
        return "ILLEGAL_F4";
    case ILLEGAL_FC:
        return "ILLEGAL_FC";
    case ILLEGAL_FD:
        return "ILLEGAL_FD";
    case INC:
        return "INC";
    case JP:
        return "JP";
    case JR:
        return "JR";
    case LD:
        return "LD";
    case LDH:
        return "LDH";
    case NOP:
        return "NOP";
    case OR:
        return "OR";
    case POP:
        return "POP";
    case PREFIX:
        return "PREFIX";
    case PUSH:
        return "PUSH";
    case RET:
        return "RET";
    case RETI:
        return "RETI";
    case RLA:
        return "RLA";
    case RLCA:
        return "RLCA";
    case RRA:
        return "RRA";
    case RRCA:
        return "RRCA";
    case RST:
        return "RST";
    case SBC:
        return "SBC";
    case SCF:
        return "SCF";
    case STOP:
        return "STOP";
    case SUB:
        return "SUB";
    case XOR:
        return "XOR";
    case BIT:
        return "BIT";
    case RES:
        return "RES";
    case RL:
        return "RL";
    case RLC:
        return "RLC";
    case RR:
        return "RR";
    case RRC:
        return "RRC";
    case SET:
        return "SET";
    case SLA:
        return "SLA";
    case SRA:
        return "SRA";
    case SRL:
        return "SRL";
    case SWAP:
        return "SWAP";
    default:
        return "UNKNOWN";
    }
}

const char *operandToString(Operand operand)
{
    switch (operand)
    {
    case A:
        return "A";
    case B:
        return "B";
    case C:
        return "C";
    case D:
        return "D";
    case E:
        return "E";
    case F:
        return "F";
    case H:
        return "H";
    case L:
        return "L";
    case A16:
        return "A16";
    case A8:
        return "A8";
    case AF:
        return "AF";
    case BC:
        return "BC";
    case DE:
        return "DE";
    case E8:
        return "E8";
    case HL:
        return "HL";
    case N16:
        return "N16";
    case N8:
        return "N8";
    case NC:
        return "NC";
    case NULL_OP:
        return "NULL_OP";
    case NZ:
        return "NZ";
    case RST_00:
        return "RST_00";
    case RST_08:
        return "RST_08";
    case RST_10:
        return "RST_10";
    case RST_18:
        return "RST_18";
    case RST_20:
        return "RST_20";
    case RST_28:
        return "RST_28";
    case RST_30:
        return "RST_30";
    case RST_38:
        return "RST_38";
    case SP:
        return "SP";
    case Z:
        return "Z";
    case BIT_0:
        return "BIT_0";
    case BIT_1:
        return "BIT_1";
    case BIT_2:
        return "BIT_2";
    case BIT_3:
        return "BIT_3";
    case BIT_4:
        return "BIT_4";
    case BIT_5:
        return "BIT_5";
    case BIT_6:
        return "BIT_6";
    case BIT_7:
        return "BIT_7";
    default:
        return "UNKNOWN";
    }
}

InstructionSet get_instruction_by_opcode(u8 opcode, InstructionType type)
{
    if (type == PREFIXED_TYPE)
        return instruction_set_prefixed[opcode];
    return instruction_set[opcode];
}
