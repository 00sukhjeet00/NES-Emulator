#pragma once

#include "Mapper.hpp"

namespace NES {

class MMC1 : public Mapper {
   public:
    MMC1(std::vector<uint8_t> &prgCode, std::vector<uint8_t> &chrROM, int mirroring) : Mapper(prgCode, chrROM, mirroring) {
        controlReg.val = 0xF;
    }

    ~MMC1() override = default;
    void write(uint16_t address, uint8_t data) override;
    uint8_t read(uint16_t address) override;
    void ppuwrite(uint16_t address, uint8_t data) override;
    uint8_t ppuread(uint16_t address) override;

   private:
    //written by CPU
    uint8_t mmc1SR = 0x10;
    uint8_t prgRam[0x2000] = {0};

    //internal
    union {
        struct
        {
            unsigned mirroring : 2;
            unsigned prgRomBankMode : 2;
            unsigned chrRomBankMode : 1;
            unsigned padding : 3;
        };

        uint8_t val;
    } controlReg;

    uint8_t chrBank0;
    uint8_t chrBank1;
    uint8_t prgBank;
};

};  //namespace NES
