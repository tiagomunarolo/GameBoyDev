#include "stack.hpp"
#include "bus.hpp"

void push_stack(volatile u16 &sp, u16 value, Memory *mem)
{
    sp = sp - 1;                            // (1) Move SP down
    bus_write(sp, (u8)(value >> 8), mem);   // (2) Write HIGH byte
    sp = sp - 1;                            // (3) Move SP down again
    bus_write(sp, (u8)(value & 0xff), mem); // (4) Write LOW byte
}

u16 pop_stack(volatile u16 &sp, Memory *mem)
{
    u8 low = read_u8bit_address(sp, mem);
    sp = sp + 1;
    u8 high = read_u8bit_address(sp, mem);
    sp = sp + 1;
    return (u16)((high << 8) | low);
}
