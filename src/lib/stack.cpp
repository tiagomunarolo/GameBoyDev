#include "stack.hpp"
#include "bus.hpp"

void push_stack(u16 *sp, u16 value)
{
    *sp = *sp - 1;                      // (1) Move SP down
    bus_write(*sp, (u8)(value >> 8));   // (2) Write HIGH byte
    *sp = *sp - 1;                      // (3) Move SP down again
    bus_write(*sp, (u8)(value & 0xff)); // (4) Write LOW byte
}

u16 pop_stack(u16 *sp)
{
    u8 low = read_u8bit_address(*sp);
    *sp = *sp + 1;
    u8 high = read_u8bit_address(*sp);
    *sp = *sp + 1;
    return (u16)((high << 8) | low);
}
