#pragma once

#include <stdint.h>
#include <stdio.h>

#include <bitset>
#include <string>
#include <vector>

#include "Bus.hpp"

namespace NES
{

    class Mapper;

    struct Header
    {
        // Header 16 byte
        char nes[4];
        uint8_t prgIn16kb;
        uint8_t chrIn8kb;
        uint8_t flags6;
        uint8_t flags7;
        uint8_t prgRamIn8kb;
        uint8_t flags9;
        uint8_t flags10;
        char zeros[5];
    };

    class ROM
    {
    public:
        std::vector<uint8_t> getChrData() { return chrData; };
        std::vector<uint8_t> getPrgCode() { return prgCode; };
        void open(std::string);
        void printHeader();
        int getMirroring();
        Mapper *getMapper();

    private:
        Header header;
        std::vector<uint8_t> trainer;
        std::vector<uint8_t> prgCode;
        std::vector<uint8_t> chrData;
        int mirroring;
        uint8_t mapperNum;
    };

}; // namespace NES
