#include "Ram.hpp"
namespace NES
{
    uint8_t Ram::read(uint16_t address)
    {
        address %= 2048;
        return ram[address];
    }
    void Ram::write(uint16_t address, uint8_t data)
    {
        address %= 2048;
        ram[address] = data;
    }
} // namespace NES
