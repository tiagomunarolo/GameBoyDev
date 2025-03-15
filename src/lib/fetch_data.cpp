#include "fetch_data.hpp"
#include "bus.hpp"
#include "register.hpp"

using namespace std;

void imm16_to_r16(u16 addreess, CPU *cpu, Memory *mem)
{ // Handle IMM16_TO_R16
    InstructionSet in = cpu->current_instruction;
    u16 value = read_u16bit_address(addreess, mem);
    set_register_value(cpu, in.op1, value);
}

void imm8_to_r16(u16 addreess, CPU *cpu, Memory *mem)
{ // Handle IMM16_TO_R16
    InstructionSet in = cpu->current_instruction;
    if (cpu->opcode == 0xE8)
    { // ADD SP, E8
        cpu->fetched_data = read_8bit_address(addreess, mem);
        return;
    }
    u8 value = read_u8bit_address(addreess, mem);
    u16 reg_pointer = fetch_reg16(cpu, in.op1);
    bus_write(reg_pointer, value, mem);
}

void imm8_to_r8(u16 addreess, CPU *cpu, Memory *mem)
{ // Handle IMM16_TO_R16
    InstructionSet in = cpu->current_instruction;
    if (in.mnemonic == LD)
    {
        u8 value = read_u8bit_address(addreess, mem);
        set_register_value(cpu, in.op1, value);
    }
    else
    {
        cpu->fetched_data = read_u8bit_address(addreess, mem);
    }
}

void mem_imm16_to_r8(u16 addreess, CPU *cpu,
                     Memory *mem)
{ // Handle IMM16_TO_R16
    InstructionSet in = cpu->current_instruction;
    u16 imm16 = read_u16bit_address(addreess, mem);
    u8 content = read_8bit_address(imm16, mem);
    set_register_value(cpu, in.op1, content);
}

void mem_reg16_to_r8(u16 addreess, CPU *cpu,
                     Memory *mem)
{ // Handle IMM16_TO_R16
    InstructionSet in = cpu->current_instruction;
    u16 reg_value = fetch_reg16(cpu, in.op2);
    u8 value = read_8bit_address(reg_value, mem);
    if (cpu->opcode == 0x2A) // LD A, [HL+]
        set_register_value(cpu, in.op2, (u16)(reg_value + 1));
    else if (cpu->opcode == 0x3A) // LD A, [HL-]
        set_register_value(cpu, in.op2, (u16)(reg_value - 1));
    else if (cpu->opcode == 0xBE)
    { // CP A, [HL]
        cpu->fetched_data = read_u8bit_address(reg_value, mem);
        return;
    }
    else if (cpu->opcode == 0x8E)
    { // ADC A, [HL]
        cpu->fetched_data = read_u8bit_address(reg_value, mem);
        return;
    }
    else if (cpu->opcode == 0x86)
    { // ADD A (HL)
        cpu->fetched_data = read_u8bit_address(reg_value, mem);
        return;
    }
    else if (cpu->opcode == 0x96)
    { // SUB A (HL)
        cpu->fetched_data = read_u8bit_address(reg_value, mem);
        return;
    }
    else if (cpu->opcode == 0x9E)
    { // SBC A (HL)
        cpu->fetched_data = read_u8bit_address(reg_value, mem);
        return;
    }
    else if (cpu->opcode == 0xA6)
    { // AND (HL)
        cpu->fetched_data = read_u8bit_address(reg_value, mem);
        return;
    }
    else if (cpu->opcode == 0xAE || cpu->opcode == 0xB6)
    { // XOR A, [HL]
        cpu->fetched_data = value;
        return;
    }
    set_register_value(cpu, in.op1, value);
}

void reg16_to_imm16(u16 addreess, CPU *cpu,
                    Memory *mem)
{ // Handle IMM16_TO_R16
    InstructionSet in = cpu->current_instruction;
    u16 imm16 = read_u16bit_address(addreess, mem);
    u16 reg_value = fetch_reg16(cpu, in.op2);
    bus_write16(imm16, reg_value, mem);
}

void r8_to_imm16(u16 addreess, CPU *cpu, Memory *mem)
{ // Handle IMM16_TO_R16
    InstructionSet in = cpu->current_instruction;
    u16 imm16 = read_u16bit_address(addreess, mem);
    u8 reg_value = fetch_reg8(cpu, in.op2);
    bus_write(imm16, reg_value, mem);
}

void r8_to_mem_reg16(u16 addreess, CPU *cpu,
                     Memory *mem)
{ // Handle IMM16_TO_R16
    InstructionSet in = cpu->current_instruction;
    u16 register_pointer = fetch_reg16(cpu, in.op1);
    if (cpu->opcode == 0x22)
    { // ld [hl+], a
        bus_write(register_pointer, cpu->a, mem);
        set_register_value(cpu, in.op1, (u16)(register_pointer + 1));
    }
    else if (cpu->opcode == 0x32)
    { // ld [hl-], a
        bus_write(register_pointer, cpu->a, mem);
        set_register_value(cpu, in.op1, (u16)(register_pointer - 1));
    }
    else
        bus_write(register_pointer, fetch_reg8(cpu, in.op2), mem);
}

void high_mem_load(u16 addreess, CPU *cpu, Memory *mem)
{ // Handle IMM16_TO_R16
    InstructionSet in = cpu->current_instruction;
    u8 imm = read_u8bit_address(addreess, mem);
    if (cpu->opcode == 0xF0)
    { // LDH A, [A8]
        u16 bus_addr = 0xFF00 + imm;
        u8 value = read_u8bit_address(bus_addr, mem);
        set_register_value(cpu, in.op1, value);
    }
    else if (cpu->opcode == 0xE0)
    { // LDH [A8],A
        u16 bus_addr = 0xFF00 + imm;
        u8 data = fetch_reg8(cpu, in.op2);
        bus_write(bus_addr, data, mem);
    }
    else if (cpu->opcode == 0xE2)
    { // LDH [C], A
        u8 data = fetch_reg8(cpu, in.op2);
        u8 offset = fetch_reg8(cpu, in.op1);
        u16 bus_addr = 0xFF00 + offset;
        bus_write(bus_addr, data, mem);
    }
    else if (cpu->opcode == 0xF2)
    { // LDH A, [C]
        u8 offset = fetch_reg8(cpu, in.op2);
        u16 bus_addr = 0xFF00 + offset;
        u8 data = read_u8bit_address(bus_addr, mem);
        set_register_value(cpu, in.op1, data);
    }
}

void r8_to_r8(u16 addreess, CPU *cpu, Memory *mem)
{ // Handle IMM16_TO_R16
    InstructionSet in = cpu->current_instruction;
    if (in.mnemonic == LD)
        set_register_value(cpu, in.op1, fetch_reg8(cpu, in.op2));
    else
        cpu->fetched_data = fetch_reg8(cpu, in.op2);
}

void jump_call(u16 addreess, CPU *cpu, Memory *mem)
{ // Handle IMM16_TO_R16
    InstructionSet in = cpu->current_instruction;
    if (in.op1 == A16 && in.op2 == NULL_OP)
    {
        cpu->fetched_data = read_u16bit_address(addreess, mem);
    }
    else if (in.mnemonic == JR && (in.op2 == E8 || in.op1 == E8))
    {
        cpu->fetched_data = read_8bit_address(addreess, mem);
    }
    else if (in.mnemonic == JP && in.op2 == A16)
    {
        cpu->fetched_data = read_u16bit_address(addreess, mem);
    }
    else if (in.mnemonic == CALL && in.op2 == A16)
    {
        cpu->fetched_data = read_u16bit_address(addreess, mem);
    }
    else if (in.mnemonic == RST)
    {
        Operand rst = in.op1;
        if (rst == RST_00)
            cpu->fetched_data = 0x0000;
        else if (rst == RST_08)
            cpu->fetched_data = 0x0008;
        else if (rst == RST_10)
            cpu->fetched_data = 0x0010;
        else if (rst == RST_18)
            cpu->fetched_data = 0x0018;
        else if (rst == RST_20)
            cpu->fetched_data = 0x0020;
        else if (rst == RST_28)
            cpu->fetched_data = 0x0028;
        else if (rst == RST_30)
            cpu->fetched_data = 0x0030;
        else if (rst == RST_38)
            cpu->fetched_data = 0x0038;
    }
    else
    {
        throw std::runtime_error("INVALID_JP_CALL_OPERANDS");
    }
}

void r16_to_r16(u16 addreess, CPU *cpu, Memory *mem)
{ // Handle IMM16_TO_R16
    InstructionSet in = cpu->current_instruction;
    u16 value = fetch_reg16(cpu, in.op2);
    if (in.mnemonic != LD)
    {
        cpu->fetched_data = value;
        return;
    }

    if (cpu->opcode == 0xF8)
    { // LD HL, SP+e8
        int8_t imm8 = read_8bit_address(addreess, mem);
        u8 lower = (u8)value;
        value += imm8;
        cpu->zero_flag = false;
        cpu->sub_flag = false;
        cpu->half_carry_flag =
            ((lower & 0xf) + (imm8 & 0xf) + (cpu->sp & 0xf)) > 0xf;
        cpu->carry_flag = (u16)(lower + (u8)imm8) > 0xff;
    }
    set_register_value(cpu, in.op1, (u16)value);
}

void control_instruction(u16 addreess, CPU *cpu, Memory *mem)
{
    InstructionSet in = cpu->current_instruction;
    if (in.mnemonic != PREFIX)
        return;
    cpu->fetched_data = read_8bit_address(cpu->pc, mem);
}

void r8_only(u16 addreess, CPU *cpu, Memory *mem)
{
    InstructionSet in = cpu->current_instruction;
    try
    {
        cpu->fetched_data = fetch_reg8(cpu, in.op2);
    }
    catch (std::runtime_error &e)
    {
        cpu->fetched_data = 0;
    }
}

void fetch_data_and_update_registers(CPU *cpu, Memory *mem)
{
    u16 addreess = cpu->pc;
    cpu->fetched_data = 0;

    switch (cpu->current_instruction.operation)
    {
    case IMM16_TO_R16:
        imm16_to_r16(addreess, cpu, mem);
        break;
    case IMM8_TO_R16:
        imm8_to_r16(addreess, cpu, mem);
        break;
    case IMM8_TO_R8:
        imm8_to_r8(addreess, cpu, mem);
        break;
    case MEM_IMM16_TO_R8:
        mem_imm16_to_r8(addreess, cpu, mem);
        break;
    case MEM_REG16_TO_R8:
        mem_reg16_to_r8(addreess, cpu, mem);
        break;
    case R16_TO_IMM16:
        reg16_to_imm16(addreess, cpu, mem);
        break;
    case R8_TO_IMM16: // LD [a16], A
        r8_to_imm16(addreess, cpu, mem);
        break;
    case R8_TO_MEM_R16: // LD [DE], A
        r8_to_mem_reg16(addreess, cpu, mem);
        break;
    case HIGHMEM_LOAD:
        high_mem_load(addreess, cpu, mem);
        break;
    case R8_TO_R8:
        r8_to_r8(addreess, cpu, mem);
        break;
    case JUMP_CALL:
        jump_call(addreess, cpu, mem);
        break;
    case R16_ONLY: // DEC HL
        cpu->fetched_data = fetch_reg16(cpu, cpu->current_instruction.op1);
        break;
    case R16_to_R16: // ADD HL, BC
        r16_to_r16(addreess, cpu, mem);
        break;
    case CONTROL_INSTRUCTION:
        control_instruction(addreess, cpu, mem);
        break;
    case R8_ONLY:
        r8_only(addreess, cpu, mem);
        break;
    default:
        break;
    }
}
