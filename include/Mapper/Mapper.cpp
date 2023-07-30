#include "Mapper.hpp"

namespace NES
{

    uint8_t Mapper::ppuread(uint16_t address)
    {
        return chrROM[address];
    }

    void Mapper::ppuwrite(uint16_t address, uint8_t data)
    {
        chrROM[address] = data;
    }

} // namespace NES
