#pragma once

#include <stdint.h>
#include <stdio.h>
#include "Bus.hpp"

namespace NES
{
    class Ram : public NES::Bus
    {
    private:
        // 256 byte pages, 8 pages on internal NES RAM
        uint8_t ram[2048] = {0};

    public:
        uint8_t read(uint16_t address);
        void write(uint16_t address, uint8_t data);
    };

} // namespace NES
