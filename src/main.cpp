#include "bus.hpp"
#include "cart.hpp"
#include "gb.hpp"
#include "ui.hpp"
#include "pipeline.hpp"
#include <SDL.h>
#include <SDL_ttf.h>
#include <thread>

using namespace std;

void main_loop(GB *gb)
{
    UI *ui = gb->get_ui();
    // create thread for pipeline
    std::thread gb_thread(run_pipeline, gb);
    gb_thread.detach();

    while (ui->running)
    {
        ui->check_event();
        ui->update();
        SDL_Delay(100);
    }
    gb->kill();
    ui->quit();
}

int main(int argc, const char *argv[])
{
    // Create GB
    GB *gb = new GB(get_game(argc, argv));

    try
    {
        // enter main loop
        main_loop(gb);
    }
    catch (std::runtime_error &e)
    {
        cout << "ERROR EXECUTING OPCODE: " << std::hex
             << int(gb->get_cpu()->opcode) << endl;
    }
    return 0;
}
