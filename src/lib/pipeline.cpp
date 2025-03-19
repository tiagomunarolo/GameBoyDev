#include "pipeline.hpp"

#ifdef DEBUG
#include "debug.hpp"
#endif

using namespace std;

void run_pipeline(GB *gb)
{
    while (gb->thread_run())
    {
        // pipeline cycle
        gb->check_halt();        // check if cpu is halted
        gb->fetch_instruction(); // fetch next instrction
#ifdef DEBUG
        debug_memory_and_registers(gb);
#endif
        gb->serial_output();        // output serial data to terminal
        gb->fetch_data();           // fetch required data from memory
        gb->execute_step();         // process operation
        gb->check_timer_overflow(); // check if timer overflow
    }
    cout << "Exiting pipeline thread" << endl;
}
