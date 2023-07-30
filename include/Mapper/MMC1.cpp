#include "MMC1.hpp"

namespace NES
{

    void MMC1::write(uint16_t address, uint8_t data)
    {
        // prg ram region
        if (address < 0x8000)
        {
            if (!(prgBank & 0x10))
            {
                prgRam[address - 0x6000] = data;
            }

            return;
        }

        if (data & 0x80)
        {
            controlReg.val |= 0xC;
            mmc1SR = 0x10;
            return;
        }

        // Shift register is full
        if (mmc1SR & 1)
        {
            mmc1SR = (mmc1SR >> 1) | ((data & 1) << 4);

            switch (address)
            {
            case 0x8000 ... 0x9FFF:
                controlReg.val = mmc1SR;

                // Make mirroring compatible with ines mirroring
                if (controlReg.mirroring == 0)
                {
                    mirroring = 2;
                }
                else if (controlReg.mirroring == 1)
                {
                    mirroring = 3;
                }
                else if (controlReg.mirroring == 2)
                {
                    mirroring = 1;
                }
                else
                {
                    mirroring = 0;
                }

                break;

            case 0xA000 ... 0xBFFF:
                chrBank0 = mmc1SR;
                break;

            case 0xC000 ... 0xDFFF:
                chrBank1 = mmc1SR;
                break;

            case 0xE000 ... 0xFFFF:
                prgBank = mmc1SR;
                break;
            }

            mmc1SR = 0x10;
        }
        else
        {
            mmc1SR = (mmc1SR >> 1) | ((data & 1) << 4);
        }
    }

    uint8_t MMC1::read(uint16_t address)
    {
        // prg ram region

        if (address < 0x8000)
        {
            if (!(prgBank & 0x10))
            {
                return prgRam[address - 0x6000];
            }

            return 0;
        }

        // switch 32kb banks
        if (controlReg.prgRomBankMode <= 1)
        {
            // ignore low bit of bankselect
            uint8_t bankSelect = prgBank & 0xE;
            uint32_t address_32 = (address - 0x8000) + (bankSelect * 0x8000);

            return prgCode[address_32];
            // fixed first 0x8000, switch (16kb) upper
        }
        else if (controlReg.prgRomBankMode == 2)
        {
            if (address < 0xC000)
            {
                return prgCode[address - 0x8000];
            }
            else
            {
                uint8_t bankSelect = prgBank & 0xF;
                uint32_t address_32 = (address - 0xC000) + (bankSelect * 0x4000);

                return prgCode[address_32];
            }
            // Switch (16kb) 0x8000, fixed upper
        }
        else
        {
            if (address < 0xC000)
            {
                uint8_t bankSelect = prgBank & 0xF;
                uint32_t address_32 = (bankSelect * 0x4000) + (address - 0x8000);

                return prgCode[address_32];
            }
            else
            {
                return prgCode[prgCode.size() - 0x4000 + (address - 0xC000)];
            }
        }
    }

    void MMC1::ppuwrite(uint16_t address, uint8_t data)
    {
        chrROM[address] = data;
    }

    uint8_t MMC1::ppuread(uint16_t address)
    {
        // 8kb mode
        if (controlReg.chrRomBankMode == 0)
        {
            // bit0 ignored
            return chrROM[(chrBank0 & 0x1E) * 0x2000 + address];
            // 4kb mode
        }
        else
        {
            if (address < 0x1000)
            {
                return chrROM[chrBank0 * 0x1000 + address];
            }
            else
            {
                return chrROM[chrBank1 * 0x1000 + (address - 0x1000)];
            }
        }
    }

} // namespace NES
