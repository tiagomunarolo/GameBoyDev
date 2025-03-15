#include "processor.hpp"
#include "bus.hpp"
#include "register.hpp"
#include "stack.hpp"

bool check_half_carry(u8 v1, u8 v2)
{
    return ((v1 & 0x0F) + (v2 & 0x0F)) > 0x0F;
}

bool check_half_carry(u16 v1, u16 v2)
{
    return ((v1 & 0x0FFF) + (v2 & 0x0FFF)) > 0x0FFF;
}

u8 get_prefix_operator(GB *gb)
{
    InstructionSet in = gb->get_cpu()->current_instruction;
    if (in.op1 != HL && in.op2 != HL)
    {
        if (in.op2 == NULL_OP)
            return fetch_reg8(gb->get_cpu(), in.op1);
        else
            return fetch_reg8(gb->get_cpu(), in.op2);
    }

    u16 addr = fetch_reg16(gb->get_cpu(), HL);
    u8 current = read_u8bit_address(addr, gb->get_memory());
    gb->get_timer()->update_timer(in.cycles);
    return current;
}

void set_prefix_operator(GB *gb, InstructionSet in, u8 value)
{

    if (in.op1 == HL || in.op2 == HL)
        bus_write(fetch_reg16(gb->get_cpu(), HL), value, gb->get_memory());
    else
    {
        if (in.op2 == NULL_OP)
            set_register_value(gb->get_cpu(), in.op1, value);
        else
            set_register_value(gb->get_cpu(), in.op2, value);
    }
}

void execute_none(GB *gb)
{
    // do nothing
    InstructionSet in = gb->get_cpu()->current_instruction;
    gb->get_timer()->update_timer(in.cycles);
}

void int_timer_handler(GB *gb)
{
    // store old pc, because current inst was note executed due to
    // interruption priority
    push_stack(gb->get_cpu()->sp, gb->get_cpu()->old_pc, gb->get_memory());
    gb->get_cpu()->pc = 0x50;
}

// Function for CP instruction
void execute_compare(GB *gb)
{
    InstructionSet in = gb->get_cpu()->current_instruction;
    gb->get_cpu()->sub_flag = true;

    if (in.op1 == A && in.op2 == A)
    {
        gb->get_cpu()->zero_flag = true;
        gb->get_cpu()->half_carry_flag = false;
        gb->get_cpu()->carry_flag = false;
    }
    else
    {
        u8 reg_value = fetch_reg8(gb->get_cpu(), in.op1);
        gb->get_cpu()->zero_flag = reg_value == gb->get_cpu()->fetched_data;
        gb->get_cpu()->half_carry_flag =
            (reg_value & 0xf) < (gb->get_cpu()->fetched_data & 0xf);
        gb->get_cpu()->carry_flag = gb->get_cpu()->fetched_data > reg_value;
    }
    gb->get_timer()->update_timer(in.cycles);
}

void execute_call(GB *gb)
{
    InstructionSet in = gb->get_cpu()->current_instruction;

    // if action not taken, 12 tick cycles
    gb->get_timer()->update_timer(12);
    if (in.op1 == NZ && gb->get_cpu()->zero_flag)
        return;
    if (in.op1 == Z && !gb->get_cpu()->zero_flag)
        return;
    if (in.op1 == C && !gb->get_cpu()->carry_flag)
        return;
    if (in.op1 == NC && gb->get_cpu()->carry_flag)
        return;
    push_stack(gb->get_cpu()->sp, gb->get_cpu()->pc, gb->get_memory());
    gb->get_cpu()->pc = gb->get_cpu()->fetched_data;
    // if action taken, additional 12 tick cycles (3 m cycles)
    gb->get_timer()->update_timer(12);
}

void execute_jump(GB *gb)
{
    InstructionSet in = gb->get_cpu()->current_instruction;

    if (in.op1 == A16)
    {
        // unconditional jump -> 4m cycles
        gb->get_timer()->update_timer(in.cycles);
        gb->get_cpu()->pc = gb->get_cpu()->fetched_data;
    }
    else if (in.op1 == C && gb->get_cpu()->carry_flag)
    {
        gb->get_timer()->update_timer(in.cycles);
        gb->get_cpu()->pc = gb->get_cpu()->fetched_data;
    }
    else if (in.op1 == NC && !gb->get_cpu()->carry_flag)
    {
        gb->get_timer()->update_timer(in.cycles);
        gb->get_cpu()->pc = gb->get_cpu()->fetched_data;
    }
    else if (in.op1 == Z && gb->get_cpu()->zero_flag)
    {
        gb->get_timer()->update_timer(in.cycles);
        gb->get_cpu()->pc = gb->get_cpu()->fetched_data;
    }
    else if (in.op1 == NZ && !gb->get_cpu()->zero_flag)
    {
        gb->get_timer()->update_timer(in.cycles);
        gb->get_cpu()->pc = gb->get_cpu()->fetched_data;
    }
    else if (in.op1 == HL)
    {
        gb->get_timer()->update_timer(in.cycles);
        gb->get_cpu()->pc = gb->get_cpu()->fetched_data;
    }
    else
    {
        // jump with conditional not taken
        gb->get_timer()->update_timer(12);
    }
}

void execute_ret(GB *gb)
{
    InstructionSet in = gb->get_cpu()->current_instruction;

    gb->get_timer()->update_timer(8); // 2m cycles if not executed

    if (in.op1 == NZ && gb->get_cpu()->zero_flag)
        return;
    if (in.op1 == Z && !gb->get_cpu()->zero_flag)
        return;
    if (in.op1 == C && !gb->get_cpu()->carry_flag)
        return;
    if (in.op1 == NC && gb->get_cpu()->carry_flag)
        return;
    if (in.mnemonic == RETI)
        *(gb->get_interruption()->ime_flag) = true;

    gb->get_timer()->update_timer(8); // 2m additional cycles if executed
    if (in.op1 == NZ || in.op1 == Z || in.op1 == C || in.op1 == NC)
        gb->get_timer()->update_timer(4);
    gb->get_cpu()->pc = pop_stack(gb->get_cpu()->sp, gb->get_memory());
    ;
}

void execute_di(GB *gb)
{
    InstructionSet in = gb->get_cpu()->current_instruction;

    *gb->get_interruption()->ime_flag = false;
    gb->get_timer()->update_timer(in.cycles);
}

void execute_ei(GB *gb)
{
    InstructionSet in = gb->get_cpu()->current_instruction;

    *gb->get_interruption()->ime_flag = true;
    gb->get_timer()->update_timer(in.cycles);
}

void execute_xor(GB *gb)
{
    InstructionSet in = gb->get_cpu()->current_instruction;

    u8 xor1 = fetch_reg8(gb->get_cpu(), in.op1);
    u8 xor2 = 0;
    if (gb->get_cpu()->opcode == 0xAE ||
        gb->get_cpu()->opcode == 0xEE) // xor a, [hl]
        xor2 = gb->get_cpu()->fetched_data;
    else
        xor2 = fetch_reg8(gb->get_cpu(), in.op2);

    gb->get_cpu()->a = xor1 ^ xor2;
    gb->get_cpu()->zero_flag = xor1 == xor2;
    gb->get_cpu()->carry_flag = 0;
    gb->get_cpu()->sub_flag = 0;
    gb->get_cpu()->half_carry_flag = 0;
    gb->get_timer()->update_timer(in.cycles);
}

void execute_dec(GB *gb)
{
    InstructionSet in = gb->get_cpu()->current_instruction;

    if (gb->get_cpu()->opcode == 0x0B)
        set_register_value(gb->get_cpu(), in.op1,
                           gb->get_cpu()->fetched_data - 1);
    else if (gb->get_cpu()->opcode == 0x1B)
        set_register_value(gb->get_cpu(), in.op1,
                           gb->get_cpu()->fetched_data - 1);
    else if (gb->get_cpu()->opcode == 0x2B)
        set_register_value(gb->get_cpu(), in.op1,
                           gb->get_cpu()->fetched_data - 1);
    else if (gb->get_cpu()->opcode == 0x3B) // dec sp
        gb->get_cpu()->sp -= 1;

    else
    {
        gb->get_cpu()->sub_flag = 1;

        try
        {
            u16 addr = fetch_reg16(gb->get_cpu(), in.op1);
            // decrement byte pointed by r16
            u8 content = read_u16bit_address(addr, gb->get_memory());
            bus_write(addr, content - 1, gb->get_memory());
            gb->get_cpu()->zero_flag = (content - 1) == 0;
            gb->get_cpu()->half_carry_flag =
                (content & 0xf) == 0x0 && ((content - 1) & 0xf) == 0xf;
        }
        catch (const std::runtime_error &e)
        {
            u8 reg_value = fetch_reg8(gb->get_cpu(), in.op1);
            u8 value = reg_value - 1;
            set_register_value(gb->get_cpu(), in.op1, value);
            gb->get_cpu()->zero_flag = value == 0;
            // bit 4 >> sets bit 3
            gb->get_cpu()->half_carry_flag =
                (reg_value & 0xf) == 0x0 && (value & 0xf) == 0xf;
        }
    }
    gb->get_timer()->update_timer(in.cycles);
}
void execute_jr(GB *gb)
{
    InstructionSet in = gb->get_cpu()->current_instruction;

    gb->get_timer()->update_timer(8); // 2m cycles if not executed
    if (in.op1 == E8)
    {
        gb->get_cpu()->pc = gb->get_cpu()->pc + gb->get_cpu()->fetched_data;
        gb->get_timer()->update_timer(4);
    }
    else if (in.op1 == NZ && !gb->get_cpu()->zero_flag)
    {
        gb->get_cpu()->pc = gb->get_cpu()->pc + gb->get_cpu()->fetched_data;
        gb->get_timer()->update_timer(4);
    }
    else if (in.op1 == Z && gb->get_cpu()->zero_flag)
    {
        gb->get_cpu()->pc = gb->get_cpu()->pc + gb->get_cpu()->fetched_data;
        gb->get_timer()->update_timer(4);
    }
    else if (in.op1 == C && gb->get_cpu()->carry_flag)
    {
        gb->get_cpu()->pc = gb->get_cpu()->pc + gb->get_cpu()->fetched_data;
        gb->get_timer()->update_timer(4);
    }
    else if (in.op1 == NC && !gb->get_cpu()->carry_flag)
    {
        gb->get_cpu()->pc = gb->get_cpu()->pc + gb->get_cpu()->fetched_data;
        gb->get_timer()->update_timer(4);
    }
}
void execute_add(GB *gb)
{
    InstructionSet in = gb->get_cpu()->current_instruction;

    gb->get_cpu()->sub_flag = false;
    if (in.op1 == A)
    {
        int value = fetch_reg8(gb->get_cpu(), in.op1);
        u8 nibble_reg = 0x0f & value;
        u8 nibble_data = 0x0f & gb->get_cpu()->fetched_data;
        value += gb->get_cpu()->fetched_data;
        gb->get_cpu()->carry_flag = value > 0xff;
        gb->get_cpu()->zero_flag = (value & 0xff) == 0;
        gb->get_cpu()->half_carry_flag = (u8)(nibble_reg + nibble_data) > 0xf;
        set_register_value(gb->get_cpu(), in.op1, value & 0xFF);
    }
    else
    {
        int value = fetch_reg16(gb->get_cpu(), in.op1);
        u16 value_before = value;
        value += gb->get_cpu()->fetched_data;
        set_register_value(gb->get_cpu(), in.op1, value & 0xFFFF);
        if (in.op1 == SP)
        {
            gb->get_cpu()->half_carry_flag =
                (0xf & value_before) + (0xf & gb->get_cpu()->fetched_data) >
                0xf;
            gb->get_cpu()->carry_flag =
                ((u16)(value_before & 0xff) +
                 (u16)(gb->get_cpu()->fetched_data & 0xff)) > 0xff;
        }

        else
        {
            gb->get_cpu()->half_carry_flag =
                (0xfff & value_before) + (0xfff & gb->get_cpu()->fetched_data) >
                0xfff;
            gb->get_cpu()->carry_flag = value > 0xffff;
        }
        if (in.op1 == SP)
            gb->get_cpu()->zero_flag = false;
    }
    gb->get_cpu()->sub_flag = false; // for all
    gb->get_timer()->update_timer(in.cycles);
}

void execute_sub(GB *gb)
{
    InstructionSet in = gb->get_cpu()->current_instruction;

    int value = fetch_reg8(gb->get_cpu(), in.op1);
    value -= gb->get_cpu()->fetched_data;
    set_register_value(gb->get_cpu(), in.op1, value & 0xFF);
    gb->get_cpu()->carry_flag = value < 0x0;
    gb->get_cpu()->zero_flag = value == 0;
    gb->get_cpu()->half_carry_flag =
        check_half_carry((u8)value, (u8)gb->get_cpu()->fetched_data);
    gb->get_cpu()->sub_flag = true; // for all
    gb->get_timer()->update_timer(in.cycles);
}

void execute_push(GB *gb)
{
    InstructionSet in = gb->get_cpu()->current_instruction;

    u16 data = gb->get_cpu()->fetched_data;
    if (gb->get_cpu()->opcode == 0xF5)
    { // push AF
        u8 flags = gb->get_cpu()->zero_flag << 7 |
                   gb->get_cpu()->sub_flag << 6 |
                   gb->get_cpu()->half_carry_flag << 5 |
                   gb->get_cpu()->carry_flag << 4;
        data = (gb->get_cpu()->a << 8) | flags;
    }
    push_stack(gb->get_cpu()->sp, data, gb->get_memory());
    gb->get_timer()->update_timer(in.cycles);
}
void execute_pop(GB *gb)
{
    InstructionSet in = gb->get_cpu()->current_instruction;

    u16 value = pop_stack(gb->get_cpu()->sp, gb->get_memory());
    if (in.op1 == AF)
    {
        u8 nibble = (value & 0x00f0) >> 4;
        gb->get_cpu()->zero_flag = (nibble & 0b1000);
        gb->get_cpu()->sub_flag = (nibble & 0b0100);
        gb->get_cpu()->half_carry_flag = (nibble & 0b0010);
        gb->get_cpu()->carry_flag = (nibble & 0b0001);
        set_register_value(gb->get_cpu(), in.op1, value);
    }
    else
        set_register_value(gb->get_cpu(), in.op1, value);

    gb->get_timer()->update_timer(in.cycles);
}

void execute_inc(GB *gb)
{
    InstructionSet in = gb->get_cpu()->current_instruction;

    try
    {
        u16 reg_value = fetch_reg16(gb->get_cpu(), in.op1);
        if (gb->get_cpu()->opcode == 0x34)
        { // inc [hl]
            // Z 0 H
            u8 value = read_u8bit_address(reg_value, gb->get_memory());
            gb->get_cpu()->zero_flag = value == 0xff;
            gb->get_cpu()->sub_flag = false;
            gb->get_cpu()->half_carry_flag = (0xf & value) + 1 > 0xf;
            bus_write(reg_value, value + 1, gb->get_memory());
        }
        else
            set_register_value(gb->get_cpu(), in.op1, reg_value + 1);
    }
    catch (const std::runtime_error &e)
    {
        u8 reg_value = fetch_reg8(gb->get_cpu(), in.op1);
        u8 value = reg_value + 1;
        set_register_value(gb->get_cpu(), in.op1, value);
        gb->get_cpu()->sub_flag = false;
        gb->get_cpu()->zero_flag = value == 0 ? true : false;
        gb->get_cpu()->half_carry_flag = check_half_carry(reg_value, 1);
    }
    gb->get_timer()->update_timer(in.cycles);
}

void execute_or(GB *gb)
{
    InstructionSet in = gb->get_cpu()->current_instruction;

    u8 r1 = fetch_reg8(gb->get_cpu(), in.op1);
    u8 r2 = 0;
    try
    {
        r2 = fetch_reg8(gb->get_cpu(), in.op2);
    }
    catch (std::runtime_error &e)
    {
        r2 = gb->get_cpu()->fetched_data;
    }
    r1 = r1 | r2;
    set_register_value(gb->get_cpu(), in.op1, r1);
    gb->get_cpu()->zero_flag = r1 == 0;
    gb->get_cpu()->carry_flag = false;
    gb->get_cpu()->half_carry_flag = false;
    gb->get_cpu()->sub_flag = false;
    gb->get_timer()->update_timer(in.cycles);
}

void execute_rra(GB *gb)
{
    InstructionSet in = gb->get_cpu()->current_instruction;

    u8 old_bit0 = gb->get_cpu()->a & 0x01;
    // Rotate A right through the carry flag
    gb->get_cpu()->a =
        (gb->get_cpu()->a >> 1) | (gb->get_cpu()->carry_flag << 7);
    // Update the carry flag with the old bit 0 of A
    gb->get_cpu()->carry_flag = (bool)old_bit0;
    gb->get_cpu()->zero_flag = false;
    gb->get_cpu()->sub_flag = false;
    gb->get_cpu()->half_carry_flag = false;
    gb->get_timer()->update_timer(in.cycles);
}

void execute_rrc(GB *gb)
{
    InstructionSet in = gb->get_cpu()->current_instruction;

    u8 current = get_prefix_operator(gb);
    u8 old_bit0 = current & 0x01;
    u8 value = (current >> 1) | (old_bit0 << 7);
    gb->get_cpu()->carry_flag = (bool)old_bit0;
    gb->get_cpu()->zero_flag = value == 0;
    gb->get_cpu()->sub_flag = false;
    gb->get_cpu()->half_carry_flag = false;
    set_prefix_operator(gb, in, value);
    gb->get_timer()->update_timer(in.cycles);
}

void execute_rlca(GB *gb)
{
    InstructionSet in = gb->get_cpu()->current_instruction;

    u8 old_bit7 = gb->get_cpu()->a & 0x80;
    // Rotate A right through the carry flag
    gb->get_cpu()->a = (gb->get_cpu()->a << 1) | (old_bit7 >> 7);
    // Update the carry flag with the old bit 0 of A
    gb->get_cpu()->carry_flag = (bool)old_bit7;
    gb->get_cpu()->zero_flag = false;
    gb->get_cpu()->sub_flag = false;
    gb->get_cpu()->half_carry_flag = false;
    gb->get_timer()->update_timer(in.cycles);
}

void execute_rlc(GB *gb)
{
    InstructionSet in = gb->get_cpu()->current_instruction;

    u8 current = get_prefix_operator(gb);
    u8 old_bit7 = current & 0x80;
    u8 value = (current << 1) | (old_bit7 >> 7);
    gb->get_cpu()->carry_flag = (bool)old_bit7;
    gb->get_cpu()->zero_flag = value == 0;
    gb->get_cpu()->sub_flag = false;
    gb->get_cpu()->half_carry_flag = false;
    set_prefix_operator(gb, in, value);
    gb->get_timer()->update_timer(in.cycles);
}

void execute_rla(GB *gb)
{
    InstructionSet in = gb->get_cpu()->current_instruction;

    u8 old_bit7 = gb->get_cpu()->a & 0x80;
    // Rotate A right through the carry flag
    gb->get_cpu()->a =
        (gb->get_cpu()->a << 1) | (gb->get_cpu()->carry_flag ? 0x01 : 0x00);
    // Update the carry flag with the old bit 0 of A
    gb->get_cpu()->carry_flag = (bool)old_bit7;
    gb->get_cpu()->zero_flag = false;
    gb->get_cpu()->sub_flag = false;
    gb->get_cpu()->half_carry_flag = false;
    gb->get_timer()->update_timer(in.cycles);
}

void execute_rrca(GB *gb)
{
    InstructionSet in = gb->get_cpu()->current_instruction;

    u8 old_bit0 = gb->get_cpu()->a & 0x01;
    // Rotate A right through the carry flag
    gb->get_cpu()->a = (gb->get_cpu()->a >> 1) | (old_bit0 << 7);
    // Update the carry flag with the old bit 0 of A
    gb->get_cpu()->carry_flag = (bool)old_bit0;
    gb->get_cpu()->zero_flag = false;
    gb->get_cpu()->sub_flag = false;
    gb->get_cpu()->half_carry_flag = false;
    gb->get_timer()->update_timer(in.cycles);
}

void execute_adc(GB *gb)
{
    InstructionSet in = gb->get_cpu()->current_instruction;

    gb->get_cpu()->sub_flag = false;
    int value = fetch_reg8(gb->get_cpu(), in.op1);
    gb->get_cpu()->half_carry_flag =
        ((value & 0xf) + (gb->get_cpu()->fetched_data & 0xf) +
         (u8)gb->get_cpu()->carry_flag) > 0xf;
    value += gb->get_cpu()->fetched_data + (u8)gb->get_cpu()->carry_flag;
    gb->get_cpu()->zero_flag = (u8)value == 0;
    gb->get_cpu()->carry_flag = value > 0xff;
    set_register_value(gb->get_cpu(), in.op1, value & 0xFF);
    gb->get_timer()->update_timer(in.cycles);
}

void execute_and(GB *gb)
{
    InstructionSet in = gb->get_cpu()->current_instruction;

    u8 value = fetch_reg8(gb->get_cpu(), in.op1);
    value &= gb->get_cpu()->fetched_data;
    set_register_value(gb->get_cpu(), in.op1, value);
    gb->get_cpu()->zero_flag = value == 0 ? true : false;
    gb->get_cpu()->sub_flag = false;
    gb->get_cpu()->half_carry_flag = true;
    gb->get_cpu()->carry_flag = false;
    gb->get_timer()->update_timer(in.cycles);
}

void execute_daa(GB *gb)
{
    InstructionSet in = gb->get_cpu()->current_instruction;

    u8 adj = 0;
    if (gb->get_cpu()->sub_flag)
    {
        if (gb->get_cpu()->half_carry_flag)
            adj += 0x6;
        if (gb->get_cpu()->carry_flag)
            adj += 0x60;
        gb->get_cpu()->a -= adj;
    }
    else
    {
        if (gb->get_cpu()->half_carry_flag || ((gb->get_cpu()->a & 0xF) > 9))
            adj += 0x6;
        if (gb->get_cpu()->carry_flag || gb->get_cpu()->a > 0x99)
        {
            adj += 0x60;
            gb->get_cpu()->carry_flag = true;
        }
        gb->get_cpu()->a += adj;
    }

    gb->get_cpu()->zero_flag = gb->get_cpu()->a == 0;
    gb->get_cpu()->half_carry_flag = false;
    gb->get_timer()->update_timer(in.cycles);
}

void execute_prefix_srl(GB *gb)
{
    InstructionSet in = gb->get_cpu()->current_instruction;

    u8 op1 = get_prefix_operator(gb);
    u8 bit0 = op1 & 0x1;
    op1 = op1 >> 1;
    gb->get_cpu()->sub_flag = false;
    gb->get_cpu()->half_carry_flag = false;
    gb->get_cpu()->carry_flag = (bool)bit0;
    gb->get_cpu()->zero_flag = op1 == 0;
    set_prefix_operator(gb, in, op1);
    gb->get_timer()->update_timer(in.cycles);
}

void execute_prefix_rr(GB *gb)
{
    InstructionSet in = gb->get_cpu()->current_instruction;

    u8 op1 = get_prefix_operator(gb);
    u8 bit0 = op1 & 0x1;
    op1 = op1 >> 1;
    gb->get_cpu()->sub_flag = false;
    gb->get_cpu()->half_carry_flag = false;
    op1 |= (gb->get_cpu()->carry_flag) << 7;
    gb->get_cpu()->carry_flag = (bool)bit0;
    gb->get_cpu()->zero_flag = op1 == 0;
    set_prefix_operator(gb, in, op1);
    gb->get_timer()->update_timer(in.cycles);
}

void execute_prefix_rl(GB *gb)
{
    InstructionSet in = gb->get_cpu()->current_instruction;

    u8 current = get_prefix_operator(gb);
    u8 bit7 = current & 0x80;
    u8 value = (current << 1) | (gb->get_cpu()->carry_flag ? 1 : 0);
    gb->get_cpu()->sub_flag = false;
    gb->get_cpu()->half_carry_flag = false;
    gb->get_cpu()->carry_flag = (bool)bit7;
    gb->get_cpu()->zero_flag = value == 0;
    set_prefix_operator(gb, in, value);
    gb->get_timer()->update_timer(in.cycles);
}

void execute_sbc(GB *gb)
{
    InstructionSet in = gb->get_cpu()->current_instruction;

    gb->get_cpu()->sub_flag = true;
    u16 dec = (gb->get_cpu()->fetched_data + (u8)gb->get_cpu()->carry_flag);
    gb->get_cpu()->half_carry_flag =
        (gb->get_cpu()->a & 0x0f) <
        ((gb->get_cpu()->fetched_data & 0xf) + (u8)gb->get_cpu()->carry_flag);

    gb->get_cpu()->carry_flag = dec > gb->get_cpu()->a;
    gb->get_cpu()->a -= dec;
    gb->get_cpu()->zero_flag = gb->get_cpu()->a == 0;
    gb->get_timer()->update_timer(in.cycles);
}

void execute_rst(GB *gb)
{
    InstructionSet in = gb->get_cpu()->current_instruction;

    push_stack(gb->get_cpu()->sp, gb->get_cpu()->pc, gb->get_memory());
    gb->get_cpu()->pc = gb->get_cpu()->fetched_data;
    gb->get_timer()->update_timer(in.cycles);
}

void execute_cpl(GB *gb)
{
    InstructionSet in = gb->get_cpu()->current_instruction;

    gb->get_cpu()->sub_flag = true;
    gb->get_cpu()->half_carry_flag = true;
    gb->get_cpu()->a = ~gb->get_cpu()->a;
    gb->get_timer()->update_timer(in.cycles);
}

void execute_scf(GB *gb)
{
    InstructionSet in = gb->get_cpu()->current_instruction;

    gb->get_cpu()->sub_flag = false;
    gb->get_cpu()->half_carry_flag = false;
    gb->get_cpu()->carry_flag = true;
    gb->get_timer()->update_timer(in.cycles);
}

void execute_ccf(GB *gb)
{
    InstructionSet in = gb->get_cpu()->current_instruction;

    gb->get_cpu()->sub_flag = false;
    gb->get_cpu()->half_carry_flag = false;
    gb->get_cpu()->carry_flag = not gb->get_cpu()->carry_flag;
    gb->get_timer()->update_timer(in.cycles);
}

void execute_prefix_bit(GB *gb)
{
    InstructionSet in = gb->get_cpu()->current_instruction;

    u8 bit_select = u8(in.op1) - u8(BIT_0);
    u8 value = get_prefix_operator(gb);
    gb->get_cpu()->zero_flag = (value & (1 << bit_select)) == 0;
    gb->get_cpu()->sub_flag = false;
    gb->get_cpu()->half_carry_flag = true;
    gb->get_timer()->update_timer(in.cycles);
}

void execute_prefix_set(GB *gb)
{
    InstructionSet in = gb->get_cpu()->current_instruction;

    u8 bit_select = u8(in.op1) - u8(BIT_0);
    u8 value = get_prefix_operator(gb);
    value = (value | (1 << bit_select));
    set_prefix_operator(gb, in, value);
    gb->get_timer()->update_timer(in.cycles);
}

void execute_prefix_res(GB *gb)
{
    InstructionSet in = gb->get_cpu()->current_instruction;

    u8 bit_select = u8(in.op1) - u8(BIT_0);
    u8 value = get_prefix_operator(gb);
    value = (value & ~(1 << bit_select));
    set_prefix_operator(gb, in, value);
    gb->get_timer()->update_timer(in.cycles);
}

void execute_prefix_swap(GB *gb)
{
    InstructionSet in = gb->get_cpu()->current_instruction;
    u8 current = get_prefix_operator(gb);
    u8 value = ((0xf0 & current) >> 4) | ((0x0f & current) << 4);
    gb->get_cpu()->zero_flag = value == 0;
    gb->get_cpu()->sub_flag = false;
    gb->get_cpu()->half_carry_flag = false;
    gb->get_cpu()->carry_flag = false;
    set_prefix_operator(gb, in, value);
    gb->get_timer()->update_timer(in.cycles);
}

void execute_prefix_sla(GB *gb)
{
    InstructionSet in = gb->get_cpu()->current_instruction;
    u8 current = get_prefix_operator(gb);
    u8 bit7 = current & 0x80;
    u8 value = (current << 1);
    gb->get_cpu()->sub_flag = false;
    gb->get_cpu()->half_carry_flag = false;
    gb->get_cpu()->carry_flag = (bool)bit7;
    gb->get_cpu()->zero_flag = value == 0;
    set_prefix_operator(gb, in, value);
    gb->get_timer()->update_timer(in.cycles);
}

void execute_prefix_sra(GB *gb)
{
    InstructionSet in = gb->get_cpu()->current_instruction;
    u8 op1 = get_prefix_operator(gb);
    u8 bit7 = op1 & 0x80;
    u8 bit0 = op1 & 0x01;
    op1 = bit7 | (op1 >> 1);
    gb->get_cpu()->sub_flag = false;
    gb->get_cpu()->half_carry_flag = false;
    gb->get_cpu()->carry_flag = (bool)bit0;
    gb->get_cpu()->zero_flag = op1 == 0;
    set_prefix_operator(gb, in, op1);
    gb->get_timer()->update_timer(in.cycles);
}

void execute_halt(GB *gb)
{
    InstructionSet in = gb->get_cpu()->current_instruction;
    gb->get_timer()->update_timer(in.cycles);

    if (*gb->get_interruption()->ime_flag)
    {
        // do stuff
    }
    else
    {
        // ime flag disabled
        // case1
        bool case_type =
            *gb->get_interruption()->ie & *gb->get_interruption()->iflag & 0x1f;
        if (!case_type)
        {
            // case 1: enters halt mode
            gb->get_cpu()->halt = true;
            printf("halt mode: ON\n");
        }
        else
        {
            // case 2
            printf("halt BUG\n");
        }
    }
}

ProcessorFunc processor[0x100] = {[NOP] = execute_none,
                                  [CP] = execute_compare,
                                  [CALL] = execute_call,
                                  [JP] = execute_jump,
                                  [RET] = execute_ret,
                                  [DI] = execute_di,
                                  [EI] = execute_ei,
                                  [XOR] = execute_xor,
                                  [DEC] = execute_dec,
                                  [JR] = execute_jr,
                                  [ADD] = execute_add,
                                  [PUSH] = execute_push,
                                  [POP] = execute_pop,
                                  [INC] = execute_inc,
                                  [OR] = execute_or,
                                  [LD] = execute_none,
                                  [LDH] = execute_none,
                                  [SUB] = execute_sub,
                                  [RRA] = execute_rra,
                                  [RLCA] = execute_rlca,
                                  [RLC] = execute_rlc,
                                  [RRC] = execute_rrc,
                                  [ADC] = execute_adc,
                                  [STOP] = execute_none,
                                  [RETI] = execute_ret,
                                  [AND] = execute_and,
                                  [SRL] = execute_prefix_srl,
                                  [BIT] = execute_prefix_bit,
                                  [SET] = execute_prefix_set,
                                  [RES] = execute_prefix_res,
                                  [SWAP] = execute_prefix_swap,
                                  [RR] = execute_prefix_rr,
                                  [SLA] = execute_prefix_sla,
                                  [SRA] = execute_prefix_sra,
                                  [RL] = execute_prefix_rl,
                                  [DAA] = execute_daa,
                                  [SBC] = execute_sbc,
                                  [RST] = execute_rst,
                                  [CPL] = execute_cpl,
                                  [SCF] = execute_scf,
                                  [CCF] = execute_ccf,
                                  [RLA] = execute_rla,
                                  [RRCA] = execute_rrca,
                                  [HALT] = execute_halt,
                                  [INT_TIMER] = int_timer_handler

};

ProcessorFunc get_processor(Mnemonic opcode) { return processor[opcode]; }
