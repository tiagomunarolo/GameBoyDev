#include "fetch_data.hpp"
#include "bus.hpp"

using namespace std;

void imm16_to_r16(u16 addreess)
{ // Handle IMM16_TO_R16
    u16 value = read_u16bit_address(addreess);
    cpu->setFetchedData(value);
}

void imm8_to_r16(u16 addreess)
{ // Handle IMM16_TO_R16
    if (cpu->getOpcode() == 0xE8)
    { // ADD SP, E8
        cpu->setFetchedData(read_8bit_address(addreess));
        return;
    }
    cpu->setFetchedData(read_u8bit_address(addreess));
}

void imm8_to_r8(u16 addreess)
{ // Handle IMM16_TO_R16
    cpu->setFetchedData(read_u8bit_address(addreess));
}

void mem_imm16_to_r8(u16 addreess)
{ // Handle IMM16_TO_R16
    u16 imm16 = read_u16bit_address(addreess);
    u8 content = read_8bit_address(imm16);
    cpu->setFetchedData(content);
}

void mem_reg16_to_r8()
{ // Handle IMM16_TO_R16
    InstructionSet in = cpu->getInstruction();
    u16 reg_value = cpu->getRegister(in.op2);
    u8 value = read_8bit_address(reg_value);
    if (cpu->getOpcode() == 0xBE)
    { // CP A, [HL]
        cpu->setFetchedData(read_u8bit_address(reg_value));
        return;
    }
    else if (cpu->getOpcode() == 0x8E)
    { // ADC A, [HL]
        cpu->setFetchedData(read_u8bit_address(reg_value));
        return;
    }
    else if (cpu->getOpcode() == 0x86)
    { // ADD A (HL)
        cpu->setFetchedData(read_u8bit_address(reg_value));
        return;
    }
    else if (cpu->getOpcode() == 0x96)
    { // SUB A (HL)
        cpu->setFetchedData(read_u8bit_address(reg_value));
        return;
    }
    else if (cpu->getOpcode() == 0x9E)
    { // SBC A (HL)
        cpu->setFetchedData(read_u8bit_address(reg_value));
        return;
    }
    else if (cpu->getOpcode() == 0xA6)
    { // AND (HL)
        cpu->setFetchedData(read_u8bit_address(reg_value));
        return;
    }
    else if (cpu->getOpcode() == 0xAE || cpu->getOpcode() == 0xB6)
    { // XOR A, [HL]
        cpu->setFetchedData(value);
        return;
    }
}

void reg16_to_imm16(u16 addreess)
{ // Handle IMM16_TO_R16
    cpu->setFetchedData(read_u16bit_address(addreess));
}

void r8_to_imm16(u16 addreess)
{ // Handle IMM16_TO_R16
    cpu->setFetchedData(read_u16bit_address(addreess));
}

void r8_to_mem_reg16()
{ // Handle IMM16_TO_R16
    InstructionSet in = cpu->getInstruction();
    if (cpu->getOpcode() == 0x22)
        cpu->setFetchedData(cpu->getRegister(A));
    else if (cpu->getOpcode() == 0x32)
        cpu->setFetchedData(cpu->getRegister(A));
    else
        cpu->setFetchedData(cpu->getRegister(in.op2));
}

void high_mem_load(u16 addreess)
{ // Handle IMM16_TO_R16
    u8 imm = read_u8bit_address(addreess);
    if (cpu->getOpcode() == 0xF0)
    { // LDH A, [A8]
        u16 bus_addr = 0xFF00 + imm;
        u8 value = read_u8bit_address(bus_addr);
        cpu->setFetchedData(value);
    }
    else if (cpu->getOpcode() == 0xE0)
        cpu->setFetchedData(0xFF00 + imm);
}

void r8_to_r8()
{ // Handle IMM16_TO_R16
    InstructionSet in = cpu->getInstruction();
    cpu->setFetchedData(cpu->getRegister(in.op2));
}

void jump_call(u16 addreess)
{ // Handle IMM16_TO_R16
    InstructionSet in = cpu->getInstruction();
    auto op1 = std::get_if<operand_types>(&in.op1);
    auto op2 = std::get_if<operand_types>(&in.op2);
    if (op1 && *op1 == A16 && op2 && *op2 == NULL_OP)
    {
        cpu->setFetchedData(read_u16bit_address(addreess));
    }
    else if (in.mnemonic == JR && (*op2 == E8 || *op1 == E8))
    {
        cpu->setFetchedData(read_8bit_address(addreess));
    }
    else if (in.mnemonic == JP && *op2 == A16)
    {
        cpu->setFetchedData(read_u16bit_address(addreess));
    }
    else if (in.mnemonic == CALL && *op2 == A16)
    {
        cpu->setFetchedData(read_u16bit_address(addreess));
    }
    else if (in.mnemonic == RST)
    {
        auto rst = std::get_if<operand_types>(&in.op1);
        if (*rst == RST_00)
            cpu->setFetchedData(0x0000);
        else if (*rst == RST_08)
            cpu->setFetchedData(0x0008);
        else if (*rst == RST_10)
            cpu->setFetchedData(0x0010);
        else if (*rst == RST_18)
            cpu->setFetchedData(0x0018);
        else if (*rst == RST_20)
            cpu->setFetchedData(0x0020);
        else if (*rst == RST_28)
            cpu->setFetchedData(0x0028);
        else if (*rst == RST_30)
            cpu->setFetchedData(0x0030);
        else if (*rst == RST_38)
            cpu->setFetchedData(0x0038);
    }
    else
    {
        throw std::runtime_error("INVALID_JP_CALL_OPERANDS");
    }
}

void r16_to_r16(u16 addreess)
{ // Handle IMM16_TO_R16
    InstructionSet in = cpu->getInstruction();
    u16 value = cpu->getRegister(in.op2);
    cpu->setFetchedData(value);
    if (cpu->getOpcode() == 0xF8)
        cpu->setFetchedData(addreess);
}

void control_instruction()
{
    InstructionSet in = cpu->getInstruction();
    if (in.mnemonic != PREFIX)
        return;
    cpu->setFetchedData(read_8bit_address(cpu->getPC()));
}

void r8_only()
{
    InstructionSet in = cpu->getInstruction();
    cpu->setFetchedData(0);
    auto op2 = std::get_if<operand_types>(&in.op2);
    if (op2 && *op2 != NULL_OP)
        cpu->setFetchedData(cpu->getRegister(in.op2));
}

void fetch_data_and_update_registers()
{
    u16 addreess = cpu->getPC();
    cpu->setFetchedData(0);

    switch (cpu->getInstruction().operation)
    {
    case IMM16_TO_R16:
        imm16_to_r16(addreess);
        break;
    case IMM8_TO_R16:
        imm8_to_r16(addreess);
        break;
    case IMM8_TO_R8:
        imm8_to_r8(addreess);
        break;
    case MEM_IMM16_TO_R8:
        mem_imm16_to_r8(addreess);
        break;
    case MEM_REG16_TO_R8:
        mem_reg16_to_r8();
        break;
    case R16_TO_IMM16:
        reg16_to_imm16(addreess);
        break;
    case R8_TO_IMM16: // LD [a16], A
        r8_to_imm16(addreess);
        break;
    case R8_TO_MEM_R16: // LD [DE], A
        r8_to_mem_reg16();
        break;
    case HIGHMEM_LOAD:
        high_mem_load(addreess);
        break;
    case R8_TO_R8:
        r8_to_r8();
        break;
    case JUMP_CALL:
        jump_call(addreess);
        break;
    case R16_ONLY: // DEC HL
        cpu->setFetchedData(cpu->getRegister(cpu->getInstruction().op1));
        break;
    case R16_to_R16: // ADD HL, BC
        r16_to_r16(addreess);
        break;
    case CONTROL_INSTRUCTION:
        control_instruction();
        break;
    case R8_ONLY:
        r8_only();
        break;
    default:
        break;
    }
}
