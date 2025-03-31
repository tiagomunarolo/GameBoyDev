#include "cart.hpp"
#include "gb.hpp"
#ifdef UI_ENABLED
#include "ui.hpp"
#endif
#include <SDL.h>
#include <thread>

using namespace std;

void main_loop()
{
    // create thread for CPU pipeline
    std::thread cpu_thread(&CPU::run, cpu);
    cpu_thread.detach();

#ifdef UI_ENABLED
    while (ui->running)
    {
        ui->check_event();
        SDL_Delay(100);
    }
    ui->quit();
    cpu->stop();
#endif
}

int main(int argc, const char *argv[])
{
    // Create GB
    gb = new GB(get_game(argc, argv));

    try
    {
        // enter main loop
        main_loop();
    }
    catch (std::exception &e)
    {
        cout << e.what() << " Opcode: " << std::hex
             << int(gb->get_cpu()->getOpcode()) << endl;
    }
    return 0;
}
