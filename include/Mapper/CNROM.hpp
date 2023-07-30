#pragma once

#include "Mapper.hpp"

namespace NES {

class CNROM : public Mapper {
   public:
    CNROM(std::vector<uint8_t> &prgCode, std::vector<uint8_t> &chrROM, int mirroring) : Mapper(prgCode, chrROM, mirroring) {}
    ~CNROM() override = default;
    uint8_t read(uint16_t address) override;
    void write(uint16_t address, uint8_t data) override;
    uint8_t ppuread(uint16_t address) override;
    void ppuwrite(uint16_t address, uint8_t data) override;

   private:
    uint8_t bankSelect = 0;
};

};  //namespace NES
