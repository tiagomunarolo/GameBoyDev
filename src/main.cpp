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
    // SDL Event Loop (Runs in Background)
    SDL_Event event;
    UI *ui = gb->get_ui();
    // main UI loop
    std::string current("");
    // create thread for pipeline
    std::thread gb_thread(run_pipeline, gb);
    gb_thread.detach();

    while (ui->running)
    {
        while (SDL_PollEvent(&event))
            if (event.type == SDL_QUIT)
                ui->running = false;
        // compare string with previous string
        if (current != std::string(gb->serial_output()))
        {
            ui->renderText(gb->serial_output());
            current = std::string(gb->serial_output());
        }
        // sleep for 100ms to reduce cpu usage
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
