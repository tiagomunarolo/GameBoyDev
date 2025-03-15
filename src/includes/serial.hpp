#pragma once

#include "memory.hpp"

class SerialHandler
{
public:
    u8 *sb;
    u8 *sc;
    const static int buffer_size = 1024;
    char data[buffer_size] = {'\0'};
    int index;

    SerialHandler(Memory *mem);
    const char *output_serial_data();
};
