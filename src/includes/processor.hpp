#pragma once
#include "definitions.hpp"
#include "mnemonics.hpp"

// Define function pointer type
typedef void (*ProcessorFunc)();
u8 get_prefix_operator();
ProcessorFunc get_processor(Mnemonic mnemonic);
