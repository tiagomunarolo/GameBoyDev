#pragma once
#include "definitions.hpp"
#include "memory.hpp"

void push_stack(volatile u16 &sp, u16 value, Memory *mem);
u16 pop_stack(volatile u16 &sp, Memory *mem);
