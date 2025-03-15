#include "cpu.hpp"

CPU::CPU()
    : pc(0x0100), sp(0xFFFE), a(0x01), b(0x00), c(0x13), d(0x00), e(0xD8),
      f(0xB0), h(0x01), l(0x4D), zero_flag(true), sub_flag(false),
      half_carry_flag(true), carry_flag(true), opcode(0), fetched_data(0),
      halt(false) {};