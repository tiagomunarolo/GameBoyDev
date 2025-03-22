#include "serial.hpp"

using namespace std;

SerialHandler::SerialHandler(Memory *mem)
{
    sb = &mem->io[0x1];
    sc = &mem->io[0x2];
    index = 0;
}

const char *SerialHandler::output_serial_data()
{
    if (*this->sc == 0x81 && this->index <= this->buffer_size - 2)
    {
        this->data[this->index++] = *this->sb;
        *this->sc = 0x01;
        cout << this->data << endl;
    }

    return this->data[0] ? this->data : "Serial data not available";
}

SerialHandler *serial = nullptr;