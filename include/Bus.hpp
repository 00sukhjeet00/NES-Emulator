#pragma once
#include <cstdint>
#include "olc6502.hpp"
#include <iostream>
#include <array>

class Bus
{
private:
    /* data */
public:
    Bus()
    {
        for (auto &i : ram)
            i = 0x00;
        cpu.connectBus(this);
    };
    ~Bus();
    std::array<uint8_t, 64 * 1024> ram;
    olc6502 cpu;

    void write(uint16_t addr, uint8_t data);
    uint8_t read(uint16_t addr, bool bReadOnly = false);
};
