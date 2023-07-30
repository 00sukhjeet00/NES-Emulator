#include "NROM.hpp"

namespace NES {

uint8_t NROM::read(uint16_t address) {
    if (address < 0x8000) {
        return 0;
    }

    address = (address - 0x8000) % prgCode.size();
    return prgCode[address];
}

void NROM::write(uint16_t address, uint8_t data) {
    //No write in NROM
}

}  //namespace NES
