#pragma once

#include <stdint.h>
#include <stdio.h>
#include "Bus.hpp"
#include "Mapper/Mapper.hpp"
namespace NES
{
    struct Sprite
    {
        uint8_t u;
        uint8_t titleNum;
        uint8_t attr;
        uint8_t x;
        uint8_t id;
    };
    struct SpriteRenderEntity
    {
        uint8_t lo;
        uint8_t hi;
        uint8_t attr;
        uint8_t counter;
        uint8_t id;
        bool flipHorizontally;
        bool flipVertically;
        int shifted = 0;
        void shift()
        {
            if (shifted == 8)
            {
                return;
            }
            if (flipHorizontally)
            {
                lo >>= 1;
                hi >>= 1;
            }
            else
            {
                lo <<= 1;
                hi << 1;
            }
            shifted++;
        }
    };

    class PPU
    {
    public:
        PPU(){};
        uint8_t read(uint16_t address);
        void write(uint16_t address, uint8_t data);

        // PPU address space
        uint8_t ppuRead(uint16_t address);
        void ppuWrite(uint16_t address, uint8_t data);

        void tick();
        void copyOAM(uint8_t, int);
        uint8_t readOAM(int);
        bool genNMI();
        bool genrateFrame();
        void printState();
        uint32_t buffer[256 * 240] = {0};

    private:
        // Registers

        //$2000 PPUCTRL
        union
        {
            struct
            {
                unsigned baseNametableAddress : 2;
                unsigned vramAddressIncrement : 1;
                unsigned spritePatternTableAddress : 1;
                unsigned bgPatternTableAddress : 1;
                unsigned spriteSize : 1;
                unsigned ppuMasterSlaveSelect : 1;
                unsigned generateNMI : 1;
            };

            uint8_t val;
        } ppuctrl;

        //$2001 PPUMASK
        union
        {
            struct
            {
                unsigned greyScale : 1;
                unsigned showBgLeftmost8 : 1;
                unsigned showSpritesLeftmost8 : 1;
                unsigned showBg : 1;
                unsigned showSprites : 1;
                unsigned emphasizeRed : 1;
                unsigned emphasizeGreen : 1;
                unsigned emphasizeBlue : 1;
            };

            uint8_t val;
        } ppumask;

        //$2002 PPUSTATUS
        union
        {
            struct
            {
                unsigned leastSignificantBits : 5;
                unsigned spriteOverflow : 1;
                unsigned spriteZeroHit : 1;
                unsigned vBlank : 1;
            };

            uint8_t val;
        } ppustatus;

        uint8_t ppustatus_cpy = 0;
        uint8_t oamaddr = 0;   //$2003
        uint8_t oamdata = 0;   //$2004
        uint8_t ppuscroll = 0; //$2005
        uint8_t ppu_read_buffer = 0;
        uint8_t ppu_read_buffer_cpy = 0;

        uint32_t palette[64] = {
            4283716692, 4278197876, 4278718608, 4281335944, 4282646628, 4284219440, 4283696128, 4282128384,
            4280297984, 4278729216, 4278206464, 4278205440, 4278202940, 4278190080, 4278190080, 4278190080,
            4288190104, 4278734020, 4281348844, 4284227300, 4287108272, 4288681060, 4288160288, 4286069760,
            4283718144, 4280840704, 4278746112, 4278220328, 4278216312, 4278190080, 4278190080, 4278190080,
            4293717740, 4283210476, 4286086380, 4289749740, 4293154028, 4293679284, 4293683812, 4292118560,
            4288719360, 4285842432, 4283224096, 4281912428, 4281906380, 4282137660, 4278190080, 4278190080,
            4293717740, 4289252588, 4290559212, 4292129516, 4293701356, 4293701332, 4293702832, 4293182608,
            4291613304, 4290043512, 4289258128, 4288209588, 4288730852, 4288717472, 4278190080, 4278190080};

        // BG
        uint8_t bg_palette[16] = {0};
        uint8_t vram[2048] = {0};
        uint16_t v = 0, t = 0;
        uint8_t x = 0;
        int w = 0;
        uint8_t ntbyte, attrbyte, patternlow, patternhigh;
        uint16_t bgShiftRegLo;
        uint16_t bgShiftRegHi;
        uint16_t attrShiftReg1;
        uint16_t attrShiftReg2;
        uint8_t quadrant_num;

        // Sprites
        uint8_t sprite_palette[16] = {0};
        uint16_t spritePatternLowAddr, spritePatternHighAddr;
        int primaryOAMCursor = 0;
        int secondaryOAMCursor = 0;
        Sprite primaryOAM[64];
        Sprite secondaryOAM[8];
        Sprite tmpOAM;
        bool inRange = false;
        int inRangeCycles = 8;
        int spriteHeight = 8;
        std::vector<SpriteRenderEntity> spriteRenderEntities;
        SpriteRenderEntity out;

        Mapper *mapper;

        int scanLine = 0;
        int dot = 0;
        int pixelIndex = 0;
        bool odd = false;
        bool nmiOccured = false;

        // methods
        inline void copyHorizontalBits();
        inline void copyVerticalBits();
        inline bool isRenderingDisabled();
        inline void emitPixel();
        inline void fetchTiles();
        inline void xIncrement();
        inline void yIncrement();
        inline void reloadShiftersAndShift();
        inline void decrementSpriteCounters();
        uint16_t getSpritePatternAddress(const Sprite &, bool);
        void evalSprites();
        bool inYRange(const Sprite &);
        bool isUninit(const Sprite &);
    };

}; // namespace NES
