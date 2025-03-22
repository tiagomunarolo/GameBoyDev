#include "cart.hpp"
#include "gb.hpp"
#include "ui.hpp"
#include <SDL.h>
#include <SDL_ttf.h>
#include <thread>

using namespace std;

void main_loop(GB *gb)
{
    // create thread for CPU pipeline
    std::thread cpu_thread(&CPU::run, cpu);
    cpu_thread.detach();

    while (ui->running)
    {
        ui->check_event();
        ui->update();
        SDL_Delay(100);
    }
    cpu->stop();
    ui->quit();
}

int main(int argc, const char *argv[])
{
    // Create GB
    gb = new GB(get_game(argc, argv));

    try
    {
        // enter main loop
        main_loop(gb);
    }
    catch (std::runtime_error &e)
    {
        cout << "ERROR EXECUTING OPCODE: " << std::hex
             << int(gb->get_cpu()->getOpcode()) << endl;
    }
    return 0;
}
