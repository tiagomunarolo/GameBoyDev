#include "bus.hpp"
#include "cpu.hpp"
#include "debug.hpp"
#include "timer.hpp"
#include "interruption.hpp"

int run = 1;
#ifdef DEBUG_FILE
const char *file = "/Users/tiago_m2/Desktop/out.log";
std::ofstream debug_file;
#endif

void debug_memory_and_registers()
{

#ifdef DEBUG_FILE
       if (run == 1 && std::filesystem::exists(file))
       {
              std::filesystem::remove(file); // Delete file
              debug_file.open(file, std::ios::app);
       }
       else if (run == 1)
       {
              debug_file.open(file, std::ios::app);
       }

#endif
       u16 old_pc = cpu->getOldPC();
       if (old_pc == 0x0050) // timer interruption
              return;

       InstructionSet instr = cpu->getInstruction();
       const char *mnemonic_str = mnemonicToString(instr.mnemonic);
       const char *op1_str = operandToString(instr.op1);
       const char *op2_str = operandToString(instr.op2);
       printf("\n========================================================="
              "===\n");
       printf("[%d]\n", run++);
       printf("PC=[%.4X]   INS=[%s (%.2X)]   SP=[%.4X]\n", old_pc, mnemonic_str,
              cpu->getOpcode(), cpu->getRegister(SP));

       // Read memory safely
       u8 b0 = read_u8bit_address(old_pc);
       u8 b1 = read_u8bit_address(old_pc + 1);
       u8 b2 = read_u8bit_address(old_pc + 2);

       printf("MEMORY[%.4X]: %.2X %.2X %.2X\n", old_pc, b0, b1, b2);

       printf("FLAGS:  Z=[%d]  N=[%d]  H=[%d]  C=[%d]\n", cpu->getFlag(ZERO_FLAG),
              cpu->getFlag(SUB_FLAG), cpu->getFlag(HC_FLAG), cpu->getFlag(CARRY_FLAG));

       printf("REGISTERS:\n");
       printf("  AF=[%.2X%.2X]\n  BC=[%.2X%.2X]\n  DE=[%.2X%.2X]", cpu->getRegister(A), cpu->getRegister(F),
              cpu->getRegister(B), cpu->getRegister(C), cpu->getRegister(D), cpu->getRegister(E));
       printf("\n  HL=[%.2X%.2X]\n  SP=[%.4X]\n", cpu->getRegister(H), cpu->getRegister(L), cpu->getRegister(SP));
       printf("\n  IE=[%.2X]\n  IF=[%.2X]\n", interruption->getIE(), interruption->getIF());
       printf("\n  DIV=[%.2X]\n  TIMA=[%.2X]\n  TMA=[%.2X]\n  TAC=[%.2X]\n",
              timer->div, *timer->tima, *timer->tma,
              *timer->tac);

       printf("INSTRUCTION DETAILS [%s]:  Size=%d  OP1=%s, OP2=%s\n", mnemonic_str,
              instr.bytes, op1_str, op2_str);

#ifdef DEBUG_FILE

       debug_file << std::uppercase;
       debug_file << "A:" << std::hex << std::setw(2) << std::setfill('0')
                  << int(cpu->getRegister(A));
       debug_file << " F:" << std::hex << std::setw(2) << std::setfill('0')
                  << int(cpu->getRegister(F));
       debug_file << " B:" << std::hex << std::setw(2) << std::setfill('0')
                  << int(cpu->getRegister(B));
       debug_file << " C:" << std::hex << std::setw(2) << std::setfill('0')
                  << int(cpu->getRegister(C));
       debug_file << " D:" << std::hex << std::setw(2) << std::setfill('0')
                  << int(cpu->getRegister(D));
       debug_file << " E:" << std::hex << std::setw(2) << std::setfill('0')
                  << int(cpu->getRegister(E));
       debug_file << " H:" << std::hex << std::setw(2) << std::setfill('0')
                  << int(cpu->getRegister(H));
       debug_file << " L:" << std::hex << std::setw(2) << std::setfill('0')
                  << int(cpu->getRegister(L));
       debug_file << " SP:" << std::hex << std::setw(4) << std::setfill('0')
                  << int(cpu->getRegister(SP));
       debug_file << " PC:" << std::hex << std::setw(4) << std::setfill('0')
                  << int(old_pc);
       debug_file << " PCMEM:";
       debug_file << std::hex << std::setw(2) << std::setfill('0') << int(b0) << ",";
       debug_file << std::hex << std::setw(2) << std::setfill('0') << int(b1) << ",";
       debug_file << std::hex << std::setw(2) << std::setfill('0') << int(b2) << ",";
       debug_file << std::hex << std::setw(2) << std::setfill('0') << int(b3)
                  << std::endl;
#endif
}
