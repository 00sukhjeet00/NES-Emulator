#pragma once
#include <stdint.h>

#include <vector>

namespace NES
{

    class Mapper
    {
    public:
        Mapper(std::vector<uint8_t> &prgCode, std::vector<uint8_t> &chrROM, int mirroring) : prgCode(prgCode),
                                                                                             chrROM(chrROM),
                                                                                             mirroring(mirroring) {}

        virtual ~Mapper() {}
        virtual uint8_t read(uint16_t address) = 0;
        virtual void write(uint16_t address, uint8_t data) = 0;
        virtual uint8_t ppuread(uint16_t address);
        virtual void ppuwrite(uint16_t address, uint8_t data);
        int getMirroring() { return mirroring; }

    protected:
        std::vector<uint8_t> prgCode;
        std::vector<uint8_t> chrROM;
        int mirroring;
    };

}; // namespace NES
