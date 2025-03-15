#pragma once

#include "gb.hpp"

// Define function pointer type
typedef void (*ProcessorFunc)(GB *gb);

ProcessorFunc get_processor(Mnemonic mnemonic);
