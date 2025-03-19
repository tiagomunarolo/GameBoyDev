#include "memory.hpp"

static void build_wram(u8 num_banks, Memory *mem)
{
    if (num_banks == 1)
    {
        mem->wram1 = NULL;
        return;
    }
    if (num_banks == 0x0)
        num_banks = 1;
    else if (num_banks == 0x2)
        num_banks = 1;
    else if (num_banks == 0x3)
        num_banks = 4;
    else if (num_banks == 0x4)
        num_banks = 16;
    else if (num_banks == 0x5)
        num_banks = 16;
    else
        throw std::runtime_error("Invalid ram bank value");

    mem->wram1 = (u8 **)malloc(num_banks * sizeof(u8 *));

    for (uint8_t i = 0; i < num_banks; i++)
    {
        mem->wram1[i] = (u8 *)malloc(0x2000 * sizeof(u8));
    }
}

Memory::Memory(const char *rom)
{
    this->rom = rom;
    build_wram(rom[0x149], this);

    io[0] = 0xcf;

    // serial
    io[1] = 0x00;
    io[2] = 0x7e;

    // set initial vram map tiles
    for (int i = 0x9800; i <= 0x9fff; i++)
        vram[i - 0x8000] = 0x7f;

    io[3] = 0xff;
    io[8] = 0xff;
    io[9] = 0xff;
    io[10] = 0xff;
    io[11] = 0xff;
    io[12] = 0xff;
    io[13] = 0xff;
    io[14] = 0xff;
    io[15] = 0xe1;
    io[16] = 0x80;
    io[17] = 0xbf;
    io[18] = 0xf3;
    io[19] = 0xff;
    io[20] = 0xbf;
    io[21] = 0xff;
    io[22] = 0x3f;
    io[23] = 0x00;
    io[24] = 0xff;
    io[25] = 0xbf;
    io[26] = 0x7f;
    io[27] = 0xff;
    io[28] = 0x9f;
    io[29] = 0xff;
    io[30] = 0xbf;
    io[31] = 0xff;
    io[32] = 0xff;
    io[33] = 0x00;
    io[34] = 0x00;
    io[35] = 0xbf;
    io[36] = 0x77;
    io[37] = 0xf3;
    io[38] = 0xf1;
    io[39] = 0xff;
    io[40] = 0xff;
    io[41] = 0xff;
    io[42] = 0xff;
    io[43] = 0xff;
    io[44] = 0xff;
    io[45] = 0xff;
    io[46] = 0xff;
    io[47] = 0xff;
    io[48] = 0xbf;
    io[49] = 0x05;
    io[50] = 0xff;
    io[51] = 0x00;
    io[52] = 0xfe;
    io[53] = 0x12;
    io[54] = 0x77;
    io[55] = 0x00;
    io[56] = 0xff;
    io[57] = 0x01;
    io[58] = 0xef;
    io[59] = 0x0a;
    io[60] = 0xff;
    io[61] = 0x10;
    io[62] = 0xff;
    io[63] = 0x00;

    for (int i = 76; i <= 127; i++)
        io[i] = 0xff;
};