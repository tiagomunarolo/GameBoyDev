#include "processor.hpp"
#include "bus.hpp"
#include "cpu.hpp"
#include "stack.hpp"
#include "timer.hpp"
#include "interruption.hpp"

using namespace std;

static bool check_register(Operand op, Registers reg)
{
    auto isReg = std::get_if<Registers>(&op);
    return isReg && *isReg == reg;
}

static bool check_operand(Operand op, operand_types operand)
{
    auto isOperand = std::get_if<operand_types>(&op);
    return isOperand && *isOperand == operand;
}

u8 get_prefix_operator()
{
    InstructionSet in = cpu->getInstruction();
    if (!check_register(in.op1, HL) && !check_register(in.op2, HL))
    {
        if (check_operand(in.op2, NULL_OP))
            return cpu->getRegister(in.op1);
        return cpu->getRegister(in.op2);
    }

    u16 addr = cpu->getRegister(HL);
    u8 current = read_u8bit_address(addr);
    timer->update_timer(in.cycles);
    return current;
}

void set_prefix_operator(InstructionSet in, u8 value)
{

    if (check_register(in.op1, HL) || check_register(in.op2, HL))
        bus_write(cpu->getRegister(HL), value);
    else
    {
        if (check_operand(in.op2, NULL_OP))
            cpu->setRegister(in.op1, value);
        else
            cpu->setRegister(in.op2, value);
    }
}

void execute_none()
{
    // do nothing
    InstructionSet in = cpu->getInstruction();
    timer->update_timer(in.cycles);
}

void int_timer_handler()
{
    // store old pc, because current inst was note executed due to
    // interruption priority
    push_stack(cpu->getSP(), cpu->getPC());
    cpu->setPC(0x50);
}

void int_lcd_handler()
{
    // store old pc, because current inst was note executed due to
    // interruption priority
    push_stack(cpu->getSP(), cpu->getPC());
    cpu->setPC(0x48);
}

void int_vblank_handler()
{
    push_stack(cpu->getSP(), cpu->getPC());
    cpu->setPC(0x40);
}

void int_joypad_handler()
{
    push_stack(cpu->getSP(), cpu->getPC());
    cpu->setPC(0x60);
}

// Function for CP instruction
void execute_compare()
{
    InstructionSet in = cpu->getInstruction();
    cpu->setFlag(SUB_FLAG, true);

    if (check_register(in.op1, A) && check_register(in.op2, A))
    {
        cpu->setFlag(ZERO_FLAG, true);
        cpu->setFlag(HC_FLAG, false);
        cpu->setFlag(CARRY_FLAG, false);
    }
    else
    {
        u8 reg_value = cpu->getRegister(in.op1);
        cpu->setFlag(ZERO_FLAG, reg_value == cpu->getFetchedData());
        cpu->setFlag(HC_FLAG,
                     (reg_value & 0xf) < (cpu->getFetchedData() & 0xf));
        cpu->setFlag(CARRY_FLAG, cpu->getFetchedData() > reg_value);
    }
    timer->update_timer(in.cycles);
}

void execute_call()
{
    InstructionSet in = cpu->getInstruction();

    // if action not taken, 12 tick cycles
    if (check_operand(in.op1, NZ) && cpu->getFlag(ZERO_FLAG))
    {
        timer->update_timer(12);
        return;
    }
    if (check_operand(in.op1, Z) && !cpu->getFlag(ZERO_FLAG))
    {
        timer->update_timer(12);
        return;
    }
    if (check_register(in.op1, C) && !cpu->getFlag(CARRY_FLAG))
    {
        timer->update_timer(12);
        return;
    }
    if (check_operand(in.op1, NC) && cpu->getFlag(CARRY_FLAG))
    {
        timer->update_timer(12);
        return;
    }
    push_stack(cpu->getSP(), cpu->getPC());
    cpu->setPC(cpu->getFetchedData());
    // if action taken, additional 12 tick cycles (3 m cycles)
    timer->update_timer(24);
}

void execute_jump()
{
    InstructionSet in = cpu->getInstruction();

    if (check_operand(in.op1, A16))
    {
        // unconditional jump -> 4m cycles
        timer->update_timer(in.cycles);
        cpu->setPC(cpu->getFetchedData());
    }
    else if (check_register(in.op1, C) && cpu->getFlag(CARRY_FLAG))
    {
        timer->update_timer(in.cycles);
        cpu->setPC(cpu->getFetchedData());
    }
    else if (check_operand(in.op1, NC) && !cpu->getFlag(CARRY_FLAG))
    {
        timer->update_timer(in.cycles);
        cpu->setPC(cpu->getFetchedData());
    }
    else if (check_operand(in.op1, Z) && cpu->getFlag(ZERO_FLAG))
    {
        timer->update_timer(in.cycles);
        cpu->setPC(cpu->getFetchedData());
    }
    else if (check_operand(in.op1, NZ) && !cpu->getFlag(ZERO_FLAG))
    {
        timer->update_timer(in.cycles);
        cpu->setPC(cpu->getFetchedData());
    }
    else if (check_register(in.op1, HL))
    {
        timer->update_timer(in.cycles);
        cpu->setPC(cpu->getFetchedData());
    }
    else
    {
        // jump with conditional not taken
        timer->update_timer(12);
    }
}

void execute_ret()
{
    InstructionSet in = cpu->getInstruction();
    int cycles = 8;

    if (check_operand(in.op1, NZ) && cpu->getFlag(ZERO_FLAG))
    {
        timer->update_timer(cycles);
        return;
    }
    else if (check_operand(in.op1, Z) && !cpu->getFlag(ZERO_FLAG))
    {
        timer->update_timer(cycles);
        return;
    }
    else if (check_register(in.op1, C) && !cpu->getFlag(CARRY_FLAG))
    {
        timer->update_timer(cycles);
        return;
    }
    else if (check_operand(in.op1, NC) && cpu->getFlag(CARRY_FLAG))
    {
        timer->update_timer(cycles);
        return;
    }
    else if (in.mnemonic == RETI)
    {
        cycles += 8;
        cpu->setIME(true);
    }
    else if (in.mnemonic == RET && check_operand(in.op1, NULL_OP))
    {
        cycles += 8;
    }
    else
    {
        cycles += 12; // if action taken, additional 12 tick cycles
    }

    cpu->setPC(pop_stack(cpu->getSP()));
    timer->update_timer(cycles);
}

void execute_di()
{
    InstructionSet in = cpu->getInstruction();
    cpu->setIME(false);
    timer->update_timer(in.cycles);
}

void execute_ei()
{
    InstructionSet in = cpu->getInstruction();
    cpu->setIME(true);
    timer->update_timer(in.cycles);
}

void execute_xor()
{
    InstructionSet in = cpu->getInstruction();

    u8 xor1 = cpu->getRegister(in.op1);
    u8 xor2 = 0;
    if (cpu->getOpcode() == 0xAE || cpu->getOpcode() == 0xEE)
        xor2 = cpu->getFetchedData();
    else
        xor2 = cpu->getRegister(in.op2);

    cpu->setRegister(A, xor1 ^ xor2);
    cpu->setFlag(ZERO_FLAG, xor1 == xor2);
    cpu->setFlag(CARRY_FLAG, 0);
    cpu->setFlag(SUB_FLAG, 0);
    cpu->setFlag(HC_FLAG, 0);
    timer->update_timer(in.cycles);
}

void execute_dec()
{
    InstructionSet in = cpu->getInstruction();

    if (cpu->getOpcode() == 0x0B)
        cpu->setRegister(in.op1,
                         cpu->getFetchedData() - 1);
    else if (cpu->getOpcode() == 0x1B)
        cpu->setRegister(in.op1,
                         cpu->getFetchedData() - 1);
    else if (cpu->getOpcode() == 0x2B)
        cpu->setRegister(in.op1,
                         cpu->getFetchedData() - 1);
    else if (cpu->getOpcode() == 0x3B) // dec sp
        cpu->setRegister(SP, *cpu->getSP() - 1);

    else
    {
        cpu->setFlag(SUB_FLAG, 1);
        bool isReg16 = check_register(in.op1, BC) || check_register(in.op1, DE) || check_register(in.op1, HL) || check_register(in.op1, SP);

        if (isReg16)
        {
            u16 addr = cpu->getRegister(in.op1);
            // decrement byte pointed by r16
            u8 content = read_u16bit_address(addr);
            bus_write(addr, content - 1);
            cpu->setFlag(ZERO_FLAG, (content - 1) == 0);
            cpu->setFlag(HC_FLAG,
                         (content & 0xf) == 0x0 && ((content - 1) & 0xf) == 0xf);
        }
        else
        {
            u8 reg_value = cpu->getRegister(in.op1);
            u8 value = reg_value - 1;
            cpu->setRegister(in.op1, value);
            cpu->setFlag(ZERO_FLAG, value == 0);
            // bit 4 >> sets bit 3
            cpu->setFlag(HC_FLAG,
                         (reg_value & 0xf) == 0x0 && (value & 0xf) == 0xf);
        }
    }
    timer->update_timer(in.cycles);
}
void execute_jr()
{
    InstructionSet in = cpu->getInstruction();
    int cpu_cycles = 8;
    if (check_operand(in.op1, E8))
    {
        cpu->setPC(cpu->getPC() + cpu->getFetchedData());
        cpu_cycles = 12;
    }
    else if (check_operand(in.op1, NZ) && !cpu->getFlag(ZERO_FLAG))
    {
        cpu->setPC(cpu->getPC() + cpu->getFetchedData());
        cpu_cycles = 12;
    }
    else if (check_operand(in.op1, Z) && cpu->getFlag(ZERO_FLAG))
    {
        cpu->setPC(cpu->getPC() + cpu->getFetchedData());
        cpu_cycles = 12;
    }
    else if (check_register(in.op1, C) && cpu->getFlag(CARRY_FLAG))
    {
        cpu->setPC(cpu->getPC() + cpu->getFetchedData());
        cpu_cycles = 12;
    }
    else if (check_operand(in.op1, NC) && !cpu->getFlag(CARRY_FLAG))
    {
        cpu->setPC(cpu->getPC() + cpu->getFetchedData());
        cpu_cycles = 12;
    }
    timer->update_timer(cpu_cycles); // 2m cycles if not executed
}
void execute_add()
{
    InstructionSet in = cpu->getInstruction();

    cpu->setFlag(SUB_FLAG, false);
    if (check_register(in.op1, A))
    {
        int value = cpu->getRegister(in.op1);
        u8 nibble_reg = 0x0f & value;
        u8 nibble_data = 0x0f & cpu->getFetchedData();
        value += cpu->getFetchedData();
        cpu->setFlag(CARRY_FLAG, value > 0xff);
        cpu->setFlag(ZERO_FLAG, (value & 0xff) == 0);
        cpu->setFlag(HC_FLAG, (u8)(nibble_reg + nibble_data) > 0xf);
        cpu->setRegister(in.op1, value & 0xFF);
    }
    else
    {
        int value = cpu->getRegister(in.op1);
        u16 value_before = value;
        value += cpu->getFetchedData();
        cpu->setRegister(in.op1, value & 0xFFFF);
        if (check_register(in.op1, SP))
        {
            cpu->setFlag(HC_FLAG,
                         (0xf & value_before) + (0xf & cpu->getFetchedData()) >
                             0xf);
            cpu->setFlag(CARRY_FLAG,
                         ((u16)(value_before & 0xff) +
                          (u16)(cpu->getFetchedData() & 0xff)) > 0xff);
        }

        else
        {
            cpu->setFlag(HC_FLAG,
                         (0xfff & value_before) + (0xfff & cpu->getFetchedData()) >
                             0xfff);
            cpu->setFlag(CARRY_FLAG, value > 0xffff);
        }
        if (check_register(in.op1, SP))
            cpu->setFlag(ZERO_FLAG, false);
    }
    cpu->setFlag(SUB_FLAG, false); // for all
    timer->update_timer(in.cycles);
}

void execute_sub()
{
    InstructionSet in = cpu->getInstruction();

    int value = cpu->getRegister(in.op1);
    value -= cpu->getFetchedData();
    cpu->setRegister(in.op1, value & 0xFF);
    cpu->setFlag(CARRY_FLAG, value < 0x0);
    cpu->setFlag(ZERO_FLAG, value == 0);
    cpu->setFlag(HC_FLAG, ((value & 0x0F) + (cpu->getFetchedData() & 0x0F)) > 0x0F);
    cpu->setFlag(SUB_FLAG, true); // for all
    timer->update_timer(in.cycles);
}

void execute_push()
{
    InstructionSet in = cpu->getInstruction();

    u16 data = cpu->getFetchedData();
    if (cpu->getOpcode() == 0xF5)
    { // push AF
        u8 flags = cpu->getFlag(ZERO_FLAG) << 7 |
                   cpu->getFlag(SUB_FLAG) << 6 |
                   cpu->getFlag(HC_FLAG) << 5 |
                   cpu->getFlag(CARRY_FLAG) << 4;
        data = (cpu->getRegister(A) << 8) | flags;
    }
    push_stack(cpu->getSP(), data);
    timer->update_timer(in.cycles);
}
void execute_pop()
{
    InstructionSet in = cpu->getInstruction();

    u16 value = pop_stack(cpu->getSP());
    if (check_register(in.op1, AF))
    {
        u8 nibble = (value & 0x00f0) >> 4;
        cpu->setFlag(ZERO_FLAG, (nibble & 0b1000));
        cpu->setFlag(SUB_FLAG, (nibble & 0b0100));
        cpu->setFlag(HC_FLAG, (nibble & 0b0010));
        cpu->setFlag(CARRY_FLAG, (nibble & 0b0001));
        cpu->setRegister(in.op1, value);
    }
    else
        cpu->setRegister(in.op1, value);

    timer->update_timer(in.cycles);
}

void execute_inc()
{
    InstructionSet in = cpu->getInstruction();

    bool isReg16 = check_register(in.op1, BC) || check_register(in.op1, DE) || check_register(in.op1, HL) || check_register(in.op1, SP);

    if (isReg16)
    {
        u16 reg_value = cpu->getRegister(in.op1);
        if (cpu->getOpcode() == 0x34)
        { // inc [hl]
            // Z 0 H
            u8 value = read_u8bit_address(reg_value);
            cpu->setFlag(ZERO_FLAG, value == 0xff);
            cpu->setFlag(SUB_FLAG, false);
            cpu->setFlag(HC_FLAG, (0xf & value) + 1 > 0xf);
            bus_write(reg_value, value + 1);
        }
        else
            cpu->setRegister(in.op1, reg_value + 1);
    }
    else
    {
        u8 reg_value = cpu->getRegister(in.op1);
        u8 value = reg_value + 1;
        cpu->setRegister(in.op1, value);
        cpu->setFlag(SUB_FLAG, false);
        cpu->setFlag(ZERO_FLAG, value == 0 ? true : false);
        cpu->setFlag(HC_FLAG, ((reg_value & 0x0F) + 0x1) > 0x0F);
    }
    timer->update_timer(in.cycles);
}

void execute_or()
{
    InstructionSet in = cpu->getInstruction();

    u8 r1 = cpu->getRegister(in.op1);
    u8 r2 = 0;
    bool isHL = check_register(in.op2, HL);
    bool isN8 = check_operand(in.op2, N8);

    if (isHL || isN8)
        r2 = cpu->getFetchedData();
    else
        r2 = cpu->getRegister(in.op2);

    r1 = r1 | r2;
    cpu->setRegister(in.op1, r1);
    cpu->setFlag(ZERO_FLAG, r1 == 0);
    cpu->setFlag(CARRY_FLAG, false);
    cpu->setFlag(HC_FLAG, false);
    cpu->setFlag(SUB_FLAG, false);
    timer->update_timer(in.cycles);
}

void execute_rra()
{
    InstructionSet in = cpu->getInstruction();
    u8 a = cpu->getRegister(A);
    u8 old_bit0 = a & 0x01;
    // Rotate A right through the carry flag
    cpu->setRegister(A, (a >> 1) | (cpu->getFlag(CARRY_FLAG) << 7));
    // Update the carry flag with the old bit 0 of A
    cpu->setFlag(CARRY_FLAG, (bool)old_bit0);
    cpu->setFlag(ZERO_FLAG, false);
    cpu->setFlag(SUB_FLAG, false);
    cpu->setFlag(HC_FLAG, false);
    timer->update_timer(in.cycles);
}

void execute_rrc()
{
    InstructionSet in = cpu->getInstruction();

    u8 current = get_prefix_operator();
    u8 old_bit0 = current & 0x01;
    u8 value = (current >> 1) | (old_bit0 << 7);
    cpu->setFlag(CARRY_FLAG, (bool)old_bit0);
    cpu->setFlag(ZERO_FLAG, value == 0);
    cpu->setFlag(SUB_FLAG, false);
    cpu->setFlag(HC_FLAG, false);
    set_prefix_operator(in, value);
    timer->update_timer(in.cycles);
}

void execute_rlca()
{
    InstructionSet in = cpu->getInstruction();

    u8 old_bit7 = cpu->getRegister(A) & 0x80;
    // Rotate A right through the carry flag
    cpu->setRegister(A, (cpu->getRegister(A) << 1) | (old_bit7 >> 7));
    // Update the carry flag with the old bit 0 of A
    cpu->setFlag(CARRY_FLAG, (bool)old_bit7);
    cpu->setFlag(ZERO_FLAG, false);
    cpu->setFlag(SUB_FLAG, false);
    cpu->setFlag(HC_FLAG, false);
    timer->update_timer(in.cycles);
}

void execute_rlc()
{
    InstructionSet in = cpu->getInstruction();

    u8 current = get_prefix_operator();
    u8 old_bit7 = current & 0x80;
    u8 value = (current << 1) | (old_bit7 >> 7);
    cpu->setFlag(CARRY_FLAG, (bool)old_bit7);
    cpu->setFlag(ZERO_FLAG, value == 0);
    cpu->setFlag(SUB_FLAG, false);
    cpu->setFlag(HC_FLAG, false);
    set_prefix_operator(in, value);
    timer->update_timer(in.cycles);
}

void execute_rla()
{
    InstructionSet in = cpu->getInstruction();

    u8 old_bit7 = cpu->getRegister(A) & 0x80;
    u8 a = cpu->getRegister(A);
    a = (a << 1) | (cpu->getFlag(CARRY_FLAG) ? 0x01 : 0x00);
    // Rotate A right through the carry flag
    cpu->setRegister(A, a);
    // Update the carry flag with the old bit 0 of A
    cpu->setFlag(CARRY_FLAG, (bool)old_bit7);
    cpu->setFlag(ZERO_FLAG, false);
    cpu->setFlag(SUB_FLAG, false);
    cpu->setFlag(HC_FLAG, false);
    timer->update_timer(in.cycles);
}

void execute_rrca()
{
    InstructionSet in = cpu->getInstruction();

    u8 old_bit0 = cpu->getRegister(A) & 0x01;
    u8 a = (cpu->getRegister(A) >> 1) | (old_bit0 << 7);
    // Rotate A right through the carry flag
    cpu->setRegister(A, a);
    // Update the carry flag with the old bit 0 of A
    cpu->setFlag(CARRY_FLAG, (bool)old_bit0);
    cpu->setFlag(ZERO_FLAG, false);
    cpu->setFlag(SUB_FLAG, false);
    cpu->setFlag(HC_FLAG, false);
    timer->update_timer(in.cycles);
}

void execute_adc()
{
    InstructionSet in = cpu->getInstruction();

    cpu->setFlag(SUB_FLAG, false);
    int value = cpu->getRegister(in.op1);
    cpu->setFlag(HC_FLAG,
                 ((value & 0xf) + (cpu->getFetchedData() & 0xf) +
                  (u8)cpu->getFlag(CARRY_FLAG)) > 0xf);
    value += cpu->getFetchedData() + (u8)cpu->getFlag(CARRY_FLAG);
    cpu->setFlag(ZERO_FLAG, (u8)value == 0);
    cpu->setFlag(CARRY_FLAG, value > 0xff);
    cpu->setRegister(in.op1, value & 0xFF);
    timer->update_timer(in.cycles);
}

void execute_and()
{
    InstructionSet in = cpu->getInstruction();

    u8 value = cpu->getRegister(in.op1);
    value &= cpu->getFetchedData();
    cpu->setRegister(in.op1, value);
    cpu->setFlag(ZERO_FLAG, value == 0 ? true : false);
    cpu->setFlag(SUB_FLAG, false);
    cpu->setFlag(HC_FLAG, true);
    cpu->setFlag(CARRY_FLAG, false);
    timer->update_timer(in.cycles);
}

void execute_daa()
{
    InstructionSet in = cpu->getInstruction();

    u8 adj = 0;
    if (cpu->getFlag(SUB_FLAG))
    {
        if (cpu->getFlag(HC_FLAG))
            adj += 0x6;
        if (cpu->getFlag(CARRY_FLAG))
            adj += 0x60;
        u8 a = cpu->getRegister(A);
        cpu->setRegister(A, a - adj);
    }
    else
    {
        if (cpu->getFlag(HC_FLAG) || ((cpu->getRegister(A) & 0xF) > 9))
            adj += 0x6;
        if (cpu->getFlag(CARRY_FLAG) || cpu->getRegister(A) > 0x99)
        {
            adj += 0x60;
            cpu->setFlag(CARRY_FLAG, true);
        }
        u8 a = cpu->getRegister(A);
        cpu->setRegister(A, a + adj);
    }

    cpu->setFlag(ZERO_FLAG, cpu->getRegister(A) == 0);
    cpu->setFlag(HC_FLAG, false);
    timer->update_timer(in.cycles);
}

void execute_prefix_srl()
{
    InstructionSet in = cpu->getInstruction();

    u8 op1 = get_prefix_operator();
    u8 bit0 = op1 & 0x1;
    op1 = op1 >> 1;
    cpu->setFlag(SUB_FLAG, false);
    cpu->setFlag(HC_FLAG, false);
    cpu->setFlag(CARRY_FLAG, (bool)bit0);
    cpu->setFlag(ZERO_FLAG, op1 == 0);
    set_prefix_operator(in, op1);
    timer->update_timer(in.cycles);
}

void execute_prefix_rr()
{
    InstructionSet in = cpu->getInstruction();

    u8 op1 = get_prefix_operator();
    u8 bit0 = op1 & 0x1;
    op1 = op1 >> 1;
    cpu->setFlag(SUB_FLAG, false);
    cpu->setFlag(HC_FLAG, false);
    op1 |= (cpu->getFlag(CARRY_FLAG)) << 7;
    cpu->setFlag(CARRY_FLAG, (bool)bit0);
    cpu->setFlag(ZERO_FLAG, op1 == 0);
    set_prefix_operator(in, op1);
    timer->update_timer(in.cycles);
}

void execute_prefix_rl()
{
    InstructionSet in = cpu->getInstruction();

    u8 current = get_prefix_operator();
    u8 bit7 = current & 0x80;
    u8 value = (current << 1) | (cpu->getFlag(CARRY_FLAG) ? 1 : 0);
    cpu->setFlag(SUB_FLAG, false);
    cpu->setFlag(HC_FLAG, false);
    cpu->setFlag(CARRY_FLAG, (bool)bit7);
    cpu->setFlag(ZERO_FLAG, value == 0);
    set_prefix_operator(in, value);
    timer->update_timer(in.cycles);
}

void execute_sbc()
{
    InstructionSet in = cpu->getInstruction();
    u8 a = cpu->getRegister(A);
    cpu->setFlag(SUB_FLAG, true);
    u16 dec = (cpu->getFetchedData() + (u8)cpu->getFlag(CARRY_FLAG));
    bool hc_flag = (a & 0x0f) < ((cpu->getFetchedData() & 0xf) + (u8)cpu->getFlag(CARRY_FLAG));
    cpu->setFlag(HC_FLAG, hc_flag);
    cpu->setFlag(CARRY_FLAG, dec > a);
    cpu->setFlag(ZERO_FLAG, (u8)(a - dec) == 0);
    cpu->setRegister(A, a - dec);
    timer->update_timer(in.cycles);
}

void execute_rst()
{
    InstructionSet in = cpu->getInstruction();

    push_stack(cpu->getSP(), cpu->getPC());
    cpu->setPC(cpu->getFetchedData());
    timer->update_timer(in.cycles);
}

void execute_load()
{
    InstructionSet in = cpu->getInstruction();
    if (in.operation == IMM16_TO_R16)
        cpu->setRegister(in.op1, cpu->getFetchedData());
    else if (in.operation == IMM8_TO_R16)
    {
        u16 reg_pointer = cpu->getRegister(in.op1);
        bus_write(reg_pointer, cpu->getFetchedData());
    }
    else if (in.operation == IMM8_TO_R8)
        cpu->setRegister(in.op1, cpu->getFetchedData());
    else if (in.operation == MEM_IMM16_TO_R8)
        cpu->setRegister(in.op1, cpu->getFetchedData());
    else if (in.operation == MEM_REG16_TO_R8)
    {
        u16 reg_value = cpu->getRegister(in.op2);
        u8 value = read_8bit_address(reg_value);
        if (cpu->getOpcode() == 0x2A) // LD A, [HL+]
            cpu->setRegister(in.op2, (u16)(reg_value + 1));
        else if (cpu->getOpcode() == 0x3A) // LD A, [HL-]
            cpu->setRegister(in.op2, (u16)(reg_value - 1));

        cpu->setRegister(in.op1, value);
    }
    else if (in.operation == R16_TO_IMM16)
    {
        u16 reg_value = cpu->getRegister(in.op2);
        bus_write16(cpu->getFetchedData(), reg_value);
    }
    else if (in.operation == R8_TO_IMM16)
    {
        u8 reg_value = cpu->getRegister(in.op2);
        bus_write(cpu->getFetchedData(), reg_value);
    }
    else if (in.operation == R8_TO_MEM_R16)
    {
        u16 register_pointer = cpu->getRegister(*(Registers *)&in.op1);
        if (cpu->getOpcode() == 0x22)
            cpu->setRegister(in.op1, (u16)(register_pointer + 1));
        else if (cpu->getOpcode() == 0x32)
            cpu->setRegister(in.op1, (u16)(register_pointer - 1));

        bus_write(register_pointer, cpu->getFetchedData());
    }
    else if (in.operation == R16_to_R16)
    {
        u16 value = cpu->getRegister(in.op2);
        if (cpu->getOpcode() == 0xF8)
        { // LD HL, SP+e8
            int8_t imm8 = read_8bit_address(cpu->getFetchedData());
            u8 lower = (u8)value;
            value += imm8;
            cpu->setFlag(ZERO_FLAG, false);
            cpu->setFlag(SUB_FLAG, false);
            cpu->setFlag(HC_FLAG, ((lower & 0xf) + (imm8 & 0xf) + (cpu->getRegister(SP) & 0xf)) > 0xf);
            cpu->setFlag(CARRY_FLAG, (u16)(lower + (u8)imm8) > 0xff);
        }
        cpu->setRegister(in.op1, (u16)value);
    }
    else if (in.operation == R8_TO_R8)
    {
        cpu->setRegister(in.op1, cpu->getRegister(in.op2));
    }

    timer->update_timer(in.cycles);
}

void execute_ldh()
{
    InstructionSet in = cpu->getInstruction();
    if (cpu->getOpcode() == 0xF0)
    { // LDH A, [A8]
        cpu->setRegister(in.op1, cpu->getFetchedData());
    }
    else if (cpu->getOpcode() == 0xE0)
    { // LDH [A8],A
        u8 data = cpu->getRegister(in.op2);
        bus_write(cpu->getFetchedData(), data);
    }
    else if (cpu->getOpcode() == 0xE2)
    { // LDH [C], A
        u8 data = cpu->getRegister(in.op2);
        u8 offset = cpu->getRegister(in.op1);
        u16 bus_addr = 0xFF00 + offset;
        bus_write(bus_addr, data);
    }
    else if (cpu->getOpcode() == 0xF2)
    { // LDH A, [C]
        u8 offset = cpu->getRegister(in.op2);
        u16 bus_addr = 0xFF00 + offset;
        u8 data = read_u8bit_address(bus_addr);
        cpu->setRegister(in.op1, data);
    }
    timer->update_timer(in.cycles);
}

void execute_cpl()
{
    InstructionSet in = cpu->getInstruction();
    cpu->setFlag(SUB_FLAG, true);
    cpu->setFlag(HC_FLAG, true);
    cpu->setRegister(A, ~cpu->getRegister(A));
    timer->update_timer(in.cycles);
}

void execute_scf()
{
    InstructionSet in = cpu->getInstruction();
    cpu->setFlag(SUB_FLAG, false);
    cpu->setFlag(HC_FLAG, false);
    cpu->setFlag(CARRY_FLAG, true);
    timer->update_timer(in.cycles);
}

void execute_ccf()
{
    InstructionSet in = cpu->getInstruction();
    cpu->setFlag(SUB_FLAG, false);
    cpu->setFlag(HC_FLAG, false);
    cpu->setFlag(CARRY_FLAG, not cpu->getFlag(CARRY_FLAG));
    timer->update_timer(in.cycles);
}

void execute_prefix_bit()
{
    InstructionSet in = cpu->getInstruction();
    auto op1 = std::get_if<operand_types>(&in.op1);
    u8 bit_select = u8(*op1) - u8(BIT_0);
    u8 value = get_prefix_operator();
    cpu->setFlag(ZERO_FLAG, (value & (1 << bit_select)) == 0);
    cpu->setFlag(SUB_FLAG, false);
    cpu->setFlag(HC_FLAG, true);
    timer->update_timer(in.cycles);
}

void execute_prefix_set()
{
    InstructionSet in = cpu->getInstruction();
    auto op1 = std::get_if<operand_types>(&in.op1);
    u8 bit_select = u8(*op1) - u8(BIT_0);
    u8 value = get_prefix_operator();
    value = (value | (1 << bit_select));
    set_prefix_operator(in, value);
    timer->update_timer(in.cycles);
}

void execute_prefix_res()
{
    InstructionSet in = cpu->getInstruction();
    auto op1 = std::get_if<operand_types>(&in.op1);
    u8 bit_select = u8(*op1) - u8(BIT_0);
    u8 value = get_prefix_operator();
    value = (value & ~(1 << bit_select));
    set_prefix_operator(in, value);
    timer->update_timer(in.cycles);
}

void execute_prefix_swap()
{
    InstructionSet in = cpu->getInstruction();
    u8 current = get_prefix_operator();
    u8 value = ((0xf0 & current) >> 4) | ((0x0f & current) << 4);
    cpu->setFlag(ZERO_FLAG, value == 0);
    cpu->setFlag(SUB_FLAG, false);
    cpu->setFlag(HC_FLAG, false);
    cpu->setFlag(CARRY_FLAG, false);
    set_prefix_operator(in, value);
    timer->update_timer(in.cycles);
}

void execute_prefix_sla()
{
    InstructionSet in = cpu->getInstruction();
    u8 current = get_prefix_operator();
    u8 bit7 = current & 0x80;
    u8 value = (current << 1);
    cpu->setFlag(SUB_FLAG, false);
    cpu->setFlag(HC_FLAG, false);
    cpu->setFlag(CARRY_FLAG, (bool)bit7);
    cpu->setFlag(ZERO_FLAG, value == 0);
    set_prefix_operator(in, value);
    timer->update_timer(in.cycles);
}

void execute_prefix_sra()
{
    InstructionSet in = cpu->getInstruction();
    u8 op1 = get_prefix_operator();
    u8 bit7 = op1 & 0x80;
    u8 bit0 = op1 & 0x01;
    op1 = bit7 | (op1 >> 1);
    cpu->setFlag(SUB_FLAG, false);
    cpu->setFlag(HC_FLAG, false);
    cpu->setFlag(CARRY_FLAG, (bool)bit0);
    cpu->setFlag(ZERO_FLAG, op1 == 0);
    set_prefix_operator(in, op1);
    timer->update_timer(in.cycles);
}

void execute_halt()
{
    InstructionSet in = cpu->getInstruction();
    if (cpu->getIME())
    {
        // IME is enabled: normal halt behavior
        cpu->setHalt(true);
#ifdef DEBUG_CPU
        printf("HALT mode: ON\n");
#endif
    }
    else
    {
        // here, IME is disabled

        /*
        (IE & IF & 1Fh) = 0
        HALT mode is entered. It works like the IME = 1 case,
        but when a IF flag is set and the corresponding IE flag is also set,
        the CPU doesn't jump to the interrupt vector, it just continues executing instructions.
        The IF flags aren't cleared.
        */
        // interrupt_flags & enabled_interrupts & 0x1F != 0
        u8 if_reg = interruption->getIF();
        u8 ie_reg = interruption->getIE();
        bool halt = (if_reg & ie_reg & 0x1F) == 0;

        if (halt)
        {
            // halt mode on
            cpu->setHalt(true);
            printf("HALT: ON\n");
        }
        else // HALT BUG
            printf("HALT BUG\n");
    }
    timer->update_timer(in.cycles);
}

ProcessorFunc processor[0x100] = {
    [NOP] = execute_none,
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
    [LD] = execute_load,
    [LDH] = execute_ldh,
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
    [INT_TIMER] = int_timer_handler,
    [INT_LCD] = int_lcd_handler,
    [INT_VBLANK] = int_vblank_handler,
    [INT_JOYPAD] = int_joypad_handler,

};

ProcessorFunc get_processor(Mnemonic opcode) { return processor[opcode]; }
