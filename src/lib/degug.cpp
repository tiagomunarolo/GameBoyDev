#include "bus.hpp"
#include "debug.hpp"

int run = 1;
const char *file = "/Users/tiago_m2/Desktop/out.log";
std::ofstream debug_file;

void debug_memory_and_registers(GB *gb)
{
       if (run == 1 && std::filesystem::exists(file))
       {
              std::filesystem::remove(file); // Delete file
              debug_file.open(file, std::ios::app);
       }
       else if (run == 1)
       {
              debug_file.open(file, std::ios::app);
       }

       CPU *cpu = gb->get_cpu();
       Memory *mem = gb->get_memory();
       u16 old_pc = (u16)cpu->old_pc;
       if (gb->get_cpu()->old_pc == 0x0050) // timer interruption
              return;

       InstructionSet *instr = &gb->get_cpu()->current_instruction;
       const char *mnemonic_str = mnemonicToString(instr->mnemonic);
       const char *op1_str = operandToString(instr->op1);
       const char *op2_str = operandToString(instr->op2);
       printf("\n========================================================="
              "===\n");
       printf("[%d]\n", run++);
       printf("PC=[%.4X]   INS=[%s (%.2X)]   SP=[%.4X]\n", old_pc, mnemonic_str,
              cpu->opcode, cpu->sp);

       // Read memory safely
       u8 b0 = read_u8bit_address(old_pc, mem);
       u8 b1 = read_u8bit_address(old_pc + 1, mem);
       u8 b2 = read_u8bit_address(old_pc + 2, mem);
       u8 b3 = read_u8bit_address(old_pc + 3, mem);

       printf("MEMORY[%.4X]: %.2X %.2X %.2X\n", old_pc, b0, b1, b2);

       printf("FLAGS:  Z=[%d]  N=[%d]  H=[%d]  C=[%d]\n", cpu->zero_flag,
              cpu->sub_flag, cpu->half_carry_flag, cpu->carry_flag);

       printf("REGISTERS:\n");
       printf("  AF=[%.2X%.2X]\n  BC=[%.2X%.2X]\n  DE=[%.2X%.2X]", cpu->a, cpu->f,
              cpu->b, cpu->c, cpu->d, cpu->e);
       printf("\n  HL=[%.2X%.2X]\n  SP=[%.4X]\n", cpu->h, cpu->l, cpu->sp);
       printf("\n  IE=[%.2X]\n  IF=[%.2X]\n", gb->get_memory()->ie,
              gb->get_memory()->iflags);
       printf("\n  DIV=[%.2X]\n  TIMA=[%.2X]\n  TMA=[%.2X]\n  TAC=[%.2X]\n",
              gb->get_timer()->div, *gb->get_timer()->tima, *gb->get_timer()->tma,
              *gb->get_timer()->tac);

       printf("INSTRUCTION DETAILS [%s]:  Size=%d  OP1=%s, OP2=%s\n", mnemonic_str,
              instr->bytes, op1_str, op2_str);

       debug_file << std::uppercase;
       debug_file << "A:" << std::hex << std::setw(2) << std::setfill('0')
                  << int(cpu->a);
       debug_file << " F:" << std::hex << std::setw(2) << std::setfill('0')
                  << int(cpu->f);
       debug_file << " B:" << std::hex << std::setw(2) << std::setfill('0')
                  << int(cpu->b);
       debug_file << " C:" << std::hex << std::setw(2) << std::setfill('0')
                  << int(cpu->c);
       debug_file << " D:" << std::hex << std::setw(2) << std::setfill('0')
                  << int(cpu->d);
       debug_file << " E:" << std::hex << std::setw(2) << std::setfill('0')
                  << int(cpu->e);
       debug_file << " H:" << std::hex << std::setw(2) << std::setfill('0')
                  << int(cpu->h);
       debug_file << " L:" << std::hex << std::setw(2) << std::setfill('0')
                  << int(cpu->l);
       debug_file << " SP:" << std::hex << std::setw(4) << std::setfill('0')
                  << int(cpu->sp);
       debug_file << " PC:" << std::hex << std::setw(4) << std::setfill('0')
                  << int(old_pc);
       debug_file << " PCMEM:";
       debug_file << std::hex << std::setw(2) << std::setfill('0') << int(b0) << ",";
       debug_file << std::hex << std::setw(2) << std::setfill('0') << int(b1) << ",";
       debug_file << std::hex << std::setw(2) << std::setfill('0') << int(b2) << ",";
       debug_file << std::hex << std::setw(2) << std::setfill('0') << int(b3)
                  << std::endl;
}
