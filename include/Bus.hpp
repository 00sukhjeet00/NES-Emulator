#pragma once

#include <stdint.h>
#include <stdio.h>

namespace NES
{

    class Bus
    {
    public:
        virtual ~Bus() {}
        virtual uint8_t read(uint16_t address) = 0;
        virtual void write(uint16_t address, uint8_t data) = 0;
    };

}; // namespace NES
