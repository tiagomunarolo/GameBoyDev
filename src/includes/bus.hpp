#pragma once

#include "memory.hpp"
#include "definitions.hpp"

u16 read_u16bit_address(u16 address, Memory *mem);

u8 read_u8bit_address(u16 address, Memory *mem);

int8_t read_8bit_address(u16 address, Memory *mem);

void bus_write(u16 address, u8 value, Memory *mem);

void bus_write16(u16 address, u16 value, Memory *mem);
