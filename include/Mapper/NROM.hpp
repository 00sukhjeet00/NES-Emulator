#pragma once

#include "Mapper.hpp"

namespace NES {

class NROM : public Mapper {
   public:
    NROM(std::vector<uint8_t> &prgCode, std::vector<uint8_t> &chrROM, int mirroring) : Mapper(prgCode, chrROM, mirroring) {}
    ~NROM() override = default;
    uint8_t read(uint16_t address) override;
    void write(uint16_t address, uint8_t data) override;
};

};  //namespace NES
