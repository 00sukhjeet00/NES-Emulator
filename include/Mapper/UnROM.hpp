#pragma once

#include "Mapper.hpp"

namespace NES {

class UnROM : public Mapper {
   public:
    UnROM(std::vector<uint8_t> &prgCode, std::vector<uint8_t> &chrROM, int mirroring) : Mapper(prgCode, chrROM, mirroring) {
        lastBankStart = prgCode.size() - 16384;
    }
    ~UnROM() override = default;
    uint8_t read(uint16_t address) override;
    void write(uint16_t address, uint8_t data) override;

   private:
    uint8_t bankSelect = 0;
    uint32_t lastBankStart = 0;
};

};  //namespace NES
