#pragma once

#include "definitions.hpp"

enum cartridge_adress_map
{
    ENTRY_POINT = 0x100,
    LOGO = 0x104,
    TITLE = 0x134,
    MF_CODE = 0x13f,
    LICENSE_CODE = 0x144,
    CARTRIDGE_TYPE = 0x147,
    ROM_SIZE = 0x148

};

typedef struct cartridge_header
{
    u8 entry_point[0x4];
    u8 logo[0x30];
    // title includes: manufacter code (old), cgb flag
    u8 title[0x10];
    u8 new_license_code[0x2];
    u8 sgb_flag[0x1];
    u8 cart_type[0x1];
    u8 rom_size[0x1];
    u8 ram_size[0x1];
    u8 dest_code[0x1];
    u8 old_license_code[0x1];
    u8 rom_ver_number[0x1];
    u8 header_checksum[0x1];
    u8 global_checksum[0x2];
} cartridge_header;

const char *read_cartridge(const char *cartridge);
const char *get_game(int argc, const char *argv[]);