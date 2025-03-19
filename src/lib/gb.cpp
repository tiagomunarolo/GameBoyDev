#include "gb.hpp"
#include "bus.hpp"
#include "fetch_data.hpp"
#include "processor.hpp"
#include "register.hpp"

// Constructor
GB::GB(const char *rom)
{
    // thread run flag
    run = true;
    // debug step
    step = 0;
    // memory
    memory = new Memory(rom);
    // cpu
    cpu = new CPU();
    // Setup interruption registers
    interruption = new InterruptionContoller(memory);
    // timer
    timer = new TimerHanlder(memory);
    // serial
    serial = new SerialHandler(memory);
    // ppu
    ppu = new PixelProcessingUnit(memory);
    // ui
    ui = new UI(memory, ppu);
}

// Returns a pointer to the UI instance
UI *GB::get_ui() { return this->ui; }

// Returns a pointer to the CPU instance
CPU *GB::get_cpu() { return this->cpu; }

// Returns a pointer to the Memory instance
Memory *GB::get_memory() { return this->memory; }

// Returns a pointer to the InterruptionController instance
InterruptionContoller *GB::get_interruption() { return this->interruption; }

// Returns a pointer to the TimerHandler instance
TimerHanlder *GB::get_timer() { return this->timer; }

// Checks if the CPU is in a HALT state and updates the timer until an overflow
// occurs
void GB::check_halt()
{
    while (this->cpu->halt && !this->timer->tima_overflow)
        this->timer->update_timer(1);
}

// Checks for timer overflow and triggers an interruption if necessary
void GB::check_timer_overflow()
{
    if (!this->timer->tima_overflow)
        return;

    // Reset the overflow flag and trigger a timer interruption
    this->timer->tima_overflow = false;
    this->interruption->set_interruption(Timer);
}

// Outputs serial data to the terminal
const char *GB::serial_output() { return this->serial->output_serial_data(); }

// Checks if there are any pending interruptions
void GB::check_interruption()
{
    return this->interruption->check_interruption();
}

// Handles pending interruptions and executes the corresponding instruction
void GB::call_interruption()
{
    // Update the current instruction mnemonic with the pending interruption
    this->cpu->current_instruction.mnemonic =
        this->interruption->getTnterruptionMnemonic(
            this->interruption->pending_int);

    // Remove the current pending interruption
    this->interruption->pending_int = None;

    // Save the current instruction state
    InstructionSet saved = this->cpu->current_instruction;

    // Execute the interruption handler
    execute_instruction();

    // Restore the original instruction state
    this->cpu->current_instruction = saved;
}

// Determines the type of instruction being executed
void GB::set_instruction_type()
{
    // Read the opcode at the current program counter
    this->cpu->opcode = read_u8bit_address(this->cpu->pc, this->memory);

    // Check if it is a prefixed instruction
    this->cpu->prefixed = this->cpu->opcode == 0xcb;

    if (!this->cpu->prefixed)
        return;

    // Prefixed instructions are 2 bytes long, increment PC and read the new
    // opcode
    this->cpu->pc += 1;
    this->cpu->opcode = read_u8bit_address(this->cpu->pc, this->memory);
}

// Updates the currently executing instruction
void GB::update_current_instruction()
{
    this->set_instruction_type();
    InstructionType type =
        this->cpu->prefixed ? PREFIXED_TYPE : NON_PREFIXED_TYPE;
    this->cpu->current_instruction =
        get_instruction_by_opcode(this->cpu->opcode, type);
}

// Fetches the next instruction from memory
void GB::fetch_instruction()
{
    // Store the old program counter (useful for debugging)
    this->cpu->old_pc = this->cpu->pc;

    // Update the current instruction
    this->update_current_instruction();

    // Move to the next instruction in memory
    this->cpu->pc += 1;
}

// Fetches required data for the current instruction
void GB::fetch_data()
{
    u8 size = this->cpu->current_instruction.bytes;
    fetch_data_and_update_registers(this->cpu, this->memory);

    if (this->cpu->prefixed)
        return;

    // Move PC forward to account for the instruction's size
    this->cpu->pc += (size - 1);
}

// Executes the currently loaded instruction
void GB::execute_instruction()
{
    // Retrieve the function corresponding to the current instruction mnemonic
    ProcessorFunc func = get_processor(this->cpu->current_instruction.mnemonic);

    // Check for any pending interruptions before executing
    this->check_interruption();

    // If no valid function is found, throw an error
    if (!func)
        throw std::runtime_error("Unknown operation");

    // Execute the retrieved function
    func(this);

    // Update CPU flags after execution
    update_flag_register(this->cpu);
}

// Executes a single step in the CPU cycle
void GB::execute_step()
{
    // If an interruption is pending, handle it first
    if (this->interruption->pending_int != None)
    {
        this->call_interruption();
        return;
    }

    // Otherwise, execute the next instruction
    this->execute_instruction();
}

std::atomic<bool> GB::thread_run()
{
    return this->run.load();
}
void GB::kill()
{
    this->run = false;
};