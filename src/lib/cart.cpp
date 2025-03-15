#include "cart.hpp"

#include <iostream>

static std::unordered_map<u8, std::string> cartridgeTypes = {
    {0x00, "ROM ONLY"},
    {0x01, "MBC1"},
    {0x02, "MBC1+RAM"},
    {0x03, "MBC1+RAM+BATTERY"},
    {0x05, "MBC2"},
    {0x06, "MBC2+BATTERY"},
    {0x08, "ROM+RAM 9"},
    {0x09, "ROM+RAM+BATTERY 9"},
    {0x0B, "MMM01"},
    {0x0C, "MMM01+RAM"},
    {0x0D, "MMM01+RAM+BATTERY"},
    {0x0F, "MBC3+TIMER+BATTERY"},
    {0x10, "MBC3+TIMER+RAM+BATTERY 10"},
    {0x11, "MBC3"},
    {0x12, "MBC3+RAM 10"},
    {0x13, "MBC3+RAM+BATTERY 10"},
    {0x19, "MBC5"},
    {0x1A, "MBC5+RAM"},
    {0x1B, "MBC5+RAM+BATTERY"},
    {0x1C, "MBC5+RUMBLE"},
    {0x1D, "MBC5+RUMBLE+RAM"},
    {0x1E, "MBC5+RUMBLE+RAM+BATTERY"},
    {0x20, "MBC6"},
    {0x22, "MBC7+SENSOR+RUMBLE+RAM+BATTERY"},
    {0xFC, "POCKET CAMERA"},
    {0xFD, "BANDAI TAMA5"},
    {0xFE, "HuC3"},
    {0xFF, "HuC1+RAM+BATTERY"}};

static std::unordered_map<int, std::string> publisherMap = {
    {0x00, "None"},
    {0x01, "Nintendo Research & Development 1"},
    {0x08, "Capcom"},
    {0x13, "EA (Electronic Arts)"},
    {0x18, "Hudson Soft"},
    {0x19, "B-AI"},
    {0x20, "KSS"},
    {0x22, "Planning Office WADA"},
    {0x24, "PCM Complete"},
    {0x25, "San-X"},
    {0x28, "Kemco"},
    {0x29, "SETA Corporation"},
    {0x30, "Viacom"},
    {0x31, "Nintendo"},
    {0x32, "Bandai"},
    {0x33, "Ocean Software/Acclaim Entertainment"},
    {0x34, "Konami"},
    {0x35, "HectorSoft"},
    {0x37, "Taito"},
    {0x38, "Hudson Soft"},
    {0x39, "Banpresto"},
    {0x41, "Ubi Soft1"},
    {0x42, "Atlus"},
    {0x44, "Malibu Interactive"},
    {0x46, "Angel"},
    {0x47, "Bullet-Proof Software2"},
    {0x49, "Irem"},
    {0x50, "Absolute"},
    {0x51, "Acclaim Entertainment"},
    {0x52, "Activision"},
    {0x53, "Sammy USA Corporation"},
    {0x54, "Konami"},
    {0x55, "Hi Tech Expressions"},
    {0x56, "LJN"},
    {0x57, "Matchbox"},
    {0x58, "Mattel"},
    {0x59, "Milton Bradley Company"},
    {0x60, "Titus Interactive"},
    {0x61, "Virgin Games Ltd.3"},
    {0x64, "Lucasfilm Games4"},
    {0x67, "Ocean Software"},
    {0x69, "EA (Electronic Arts)"},
    {0x70, "Infogrames5"},
    {0x71, "Interplay Entertainment"},
    {0x72, "Broderbund"},
    {0x73, "Sculptured Software6"},
    {0x75, "The Sales Curve Limited7"},
    {0x78, "THQ"},
    {0x79, "Accolade"},
    {0x80, "Misawa Entertainment"},
    {0x83, "lozc"},
    {0x86, "Tokuma Shoten"},
    {0x87, "Tsukuda Original"},
    {0x91, "Chunsoft Co.8"},
    {0x92, "Video System"},
    {0x93, "Ocean Software/Acclaim Entertainment"},
    {0x95, "Varie"},
    {0x96, "Yonezawa/s’pal"},
    {0x97, "Kaneko"},
    {0x99, "Pack-In-Video"},
    {0xA4, "Konami (Yu-Gi-Oh!)"},
    {0xBL, "MTO"},
};

u16 checksum(char *rom)
{
    u16 checksum = 0;
    for (u16 address = 0x0134; address <= 0x014C; address++)
    {
        checksum = checksum - rom[address] - 1;
    }
    return checksum & 0xFF;
}

const char *read_cartridge(const char *cartridge)
{
    // open file
    std::FILE *file = fopen(cartridge, "rb");
    if (!file)
    {
        // if any error
        std::cout << "Error opening file:" << cartridge << std::endl;
        return NULL;
    }
    std::fseek(file, 0, SEEK_END); // Seek to end to get file size
    long file_size = ftell(file);
    std::cout << "GAME SIZE: " << file_size << " Bytes" << std::endl;
    std::rewind(file); // Move back to the beginning

    // Allocate memory for file content
    char *buffer = (char *)malloc(file_size + 1);
    if (!buffer)
    {
        perror("Memory allocation failed");
        fclose(file);
        return 0;
    }

    // Read entire file into buffer
    fread(buffer, 1, file_size, file);
    buffer[file_size] = '\0'; // Null-terminate for string safety

    cartridge_header *header = (cartridge_header *)(buffer + ENTRY_POINT);

    std::cout << "GAME TITLE: " << header->title << std::endl;
    auto type = cartridgeTypes.find((u8)*header->cart_type);
    std::cout << "GAME TYPE: " << (int)type->first << " " << type->second << std::endl;
    auto lic_code = publisherMap.find((u8)*header->old_license_code);
    std::cout << "GAME LIC_CODE: " << lic_code->second << std::endl;
    int rom_size = 32 * (1 << int(*header->rom_size));
    std::cout << "GAME ROM SIZE: " << rom_size << std::endl;
    int ram_size = buffer[0x149];
    std::cout << "GAME RAM SIZE: " << ram_size << std::endl;
    std::cout << "GAME ROM VERSION: " << int(*header->rom_ver_number) << std::endl;
    printf("GAME HEADER_CHECKSUM: %.2X %s\n", checksum(buffer), checksum(buffer) == *header->header_checksum ? "(PASSED)" : "(FAIL)");

    return buffer;
}

const char *get_game(int argc, const char *argv[])
{
    if (argc == 1)
    {
        std::cout << "No game provided!" << std::endl;
        exit(1);
    }

    const char *game = argv[1];
    std::cout << "GAME PATH: " << game << std::endl;

    const char *game_content = read_cartridge(game);
    if (game_content == NULL)
    {
        std::cout << "Failed to read cartridge!" << std::endl;
        exit(1);
    }

    return game_content;
}