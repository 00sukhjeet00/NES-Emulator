
#include "CNROM.hpp"

namespace NES {

uint8_t CNROM::read(uint16_t address) {
    if (address < 0x8000) {
        return 0;
    }

    address = (address - 0x8000) % prgCode.size();
    return prgCode[address];
}

void CNROM::write(uint16_t address, uint8_t data) {
    if (address < 0x8000) {
        return;
    }

    bankSelect = data & 3;
}

uint8_t CNROM::ppuread(uint16_t address) {
    uint16_t ppuaddr = bankSelect * 8192 + address;
    return chrROM[ppuaddr];
}

void CNROM::ppuwrite(uint16_t address, uint8_t data) {
}

}  //namespace NES
