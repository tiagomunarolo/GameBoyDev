#include "register.hpp"

using namespace std;

void set_register_value(CPU *cpu, Operand op, u16 value)
{
    if (op <= 7)
        *(&cpu->a + (u8)op) = (u8)value;
    else if (op == AF)
    {
        cpu->a = value >> 8;
        cpu->f = 0xF0 & value;
    }
    else if (op == BC)
    {
        cpu->b = value >> 8;
        cpu->c = 0xFF & value;
    }
    else if (op == HL)
    {
        cpu->h = value >> 8;
        cpu->l = 0xFF & value;
    }
    else if (op == DE)
    {
        cpu->d = value >> 8;
        cpu->e = 0xFF & value;
    }
    else if (op == SP)
    {
        cpu->sp = value;
    }
    else
    {
        throw std::runtime_error("Invalid Register");
    }
}

u8 fetch_reg8(CPU *cpu, Operand op)
{
    switch (op)
    {
    case A:
        return cpu->a;
    case F:
        return cpu->f;
    case B:
        return cpu->b;
    case C:
        return cpu->c;
    case D:
        return cpu->d;
    case E:
        return cpu->e;
    case L:
        return cpu->l;
    case H:
        return cpu->h;
    default:
    {
        throw std::runtime_error("Invalid 8bit Register");
    }
    }
}

u16 fetch_reg16(CPU *cpu, Operand op)
{
    switch (op)
    {
    case AF:
        return (cpu->a << 8) | cpu->f;
    case BC:
        return (cpu->b << 8) | cpu->c;
    case DE:
        return (cpu->d << 8) | cpu->e;
    case HL:
        return (cpu->h << 8) | cpu->l;
    case SP:
        return cpu->sp;
    default:
    {
        throw std::runtime_error("Invalid 16bit Register");
    }
    }
}

void update_flag_register(CPU *cpu)
{
    u8 flags = fetch_reg8(cpu, F);
    if (cpu->zero_flag)
        flags |= (1 << 7);
    else
        flags &= ~(1 << 7);

    if (cpu->sub_flag)
        flags |= (1 << 6);
    else
        flags &= ~(1 << 6);

    if (cpu->half_carry_flag)
        flags |= (1 << 5);
    else
        flags &= ~(1 << 5);

    if (cpu->carry_flag)
        flags |= (1 << 4);
    else
        flags &= ~(1 << 4);

    set_register_value(cpu, F, flags);
}
