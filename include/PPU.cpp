#include "PPU.hpp"
#include <iostream>

namespace NES
{
    void NES::PPU::tick()
    {
        if (scanLine >= 0 && scanLine <= 239 || scanLine == 261)
        {
            if (scanLine == 261)
            {
                pixelIndex = 0;
                ppustatus.val &= ~0x80;
                ppustatus.val &= ~0x20;
                ppustatus.val &= ~64;
            }
            if (dot >= 280 && dot <= 304)
            {
                copyVerticalBits();
            }
            if (scanLine >= 0 && scanLine <= 239)
            {
                evalSprites();
            }
            if (dot == 257)
            {
                copyHorizontalBits();
            }
            if ((dot >= 1 && dot <= 257) || (dot >= 321 && dot < 337))
            {
                if ((dot >= 2 && dot <= 257) || (dot >= 322 && dot <= 337))
                {
                    reloadShiftersAndShift();
                }
                if (scanLine >= 0 && scanLine <= 239)
                {
                    if (dot >= 2 && dot <= 257)
                    {
                        if (scanLine > 0)
                        {
                            decrementSpriteCounters();
                        }

                        emitPixel();
                    }
                }

                // fetch nt, at, pattern low - high
                fetchTiles();
            }
        }
        else if (scanLine >= 240 && scanLine <= 260)
        { // post-render, vblank
            if (scanLine == 240 && dot == 0)
            {
                generateFrame = true;
            }

            if (scanLine == 241 && dot == 1)
            {
                // set vbl flag
                ppustatus.val |= 0x80;

                // flag for nmi
                if (ppuctrl.val & 0x80)
                {
                    nmiOccured = true;
                }
            }
        }

        if (dot == 340)
        {
            scanLine = (scanLine + 1) % 262;
            if (scanLine == 0)
            {
                odd = !odd;
            }
            dot = 0;
        }
        else
        {
            dot++;
        }
    }
    inline void NES::PPU::xIncrement()
    {
        if ((v & 0x001F) == 31)
        {
            v &= ~0x001F;
            v ^= 0x0400;
        }
        else
        {
            v += 1;
        }
    }
    inline void NES::PPU::yIncrement()
    {
        if ((v & 0x7000) != 0x7000)
        {
            v += 0x1000;
        }
        else
        {
            v &= ~0x7000;
            int y = (v & 0x03E0) >> 5;

            if (y == 29)
            {
                y = 0;
                v ^= 0x0800;
            }
            else if (y == 31)
            {
                y = 0;
            }
            else
            {
                y += 1;
            }

            v = (v & ~0x03E0) | (y << 5);
        }
    }

    inline void NES::PPU::reloadShiftersAndShift()
    {
        if (isRenderingDisabled())
        {
            return;
        }

        bgShiftRegLo <<= 1;
        bgShiftRegHi <<= 1;
        attrShiftReg1 <<= 1;
        attrShiftReg2 <<= 1;

        if (dot % 8 == 1)
        {
            uint8_t attr_bits1 = (attrbyte >> quadrant_num) & 1;
            uint8_t attr_bits2 = (attrbyte >> quadrant_num) & 2;
            attrShiftReg1 |= attr_bits1 ? 255 : 0;
            attrShiftReg2 |= attr_bits2 ? 255 : 0;
            bgShiftRegLo |= patternlow;
            bgShiftRegHi |= patternhigh;
        }
    }

    inline bool NES::PPU::isRenderingDisabled()
    {
        return !ppumask.showBg && !ppumask.showSprites;
    }

    inline void NES::PPU::fetchTiles()
    {
        if (isRenderingDisabled())
        {
            return;
        }

        int cycle = dot % 8;

        // Fetch nametable byte
        if (cycle == 1)
        {
            ntbyte = NES::PPU::ppuRead(0x2000 | (v & 0x0FFF));
            // Fetch attribute byte, also calculate which quadrant of the attribute byte is active
        }
        else if (cycle == 3)
        {
            attrbyte = NES::PPU::ppuRead(0x23C0 | (v & 0x0C00) | ((v >> 4) & 0x38) | ((v >> 2) & 0x07));
            quadrant_num = (((v & 2) >> 1) | ((v & 64) >> 5)) * 2;
            // Get low order bits of background tile
        }
        else if (cycle == 5)
        {
            uint16_t patterAddr =
                ((uint16_t)ppuctrl.bgPatternTableAddress << 12) +
                ((uint16_t)ntbyte << 4) +
                ((v & 0x7000) >> 12);
            patternlow = NES::PPU::ppuRead(patterAddr);
            // Get high order bits of background tile
        }
        else if (cycle == 7)
        {
            uint16_t patterAddr =
                ((uint16_t)ppuctrl.bgPatternTableAddress << 12) +
                ((uint16_t)ntbyte << 4) +
                ((v & 0x7000) >> 12) + 8;
            patternhigh = NES::PPU::ppuRead(patterAddr);
            // Change columns, change rows
        }
        else if (cycle == 0)
        {
            if (dot == 256)
            {
                yIncrement();
            }

            xIncrement();
        }
    }

    inline void NES::PPU::emitPixel()
    {
        if (isRenderingDisabled())
        {
            pixelIndex++;
            return;
        }

        // Bg
        uint16_t fineSelect = 0x8000 >> x;
        uint16_t pixel1 = (bgShiftRegLo & fineSelect) << x;
        uint16_t pixel2 = (bgShiftRegHi & fineSelect) << x;
        uint16_t pixel3 = (attrShiftReg1 & fineSelect) << x;
        uint16_t pixel4 = (attrShiftReg2 & fineSelect) << x;
        uint8_t bgBit12 = (pixel2 >> 14) | (pixel1 >> 15);

        // Sprites
        uint8_t spritePixel1 = 0;
        uint8_t spritePixel2 = 0;
        uint8_t spritePixel3 = 0;
        uint8_t spritePixel4 = 0;
        uint8_t spriteBit12 = 0;
        uint8_t paletteIndex = 0 | (pixel4 >> 12) | (pixel3 >> 13) | (pixel2 >> 14) | (pixel1 >> 15);
        uint8_t spritePaletteIndex = 0;
        bool showSprite = false;
        bool spriteFound = false;

        for (auto &sprite : spriteRenderEntities)
        {
            if (sprite.counter == 0 && sprite.shifted != 8)
            {
                if (spriteFound)
                {
                    sprite.shift();
                    continue;
                }

                spritePixel1 = sprite.flipHorizontally ? ((sprite.lo & 1) << 7) : sprite.lo & 128;
                spritePixel2 = sprite.flipHorizontally ? ((sprite.hi & 1) << 7) : sprite.hi & 128;
                spritePixel3 = sprite.attr & 1;
                spritePixel4 = sprite.attr & 2;
                spriteBit12 = (spritePixel2 >> 6) | (spritePixel1 >> 7);

                // Sprite zero hit
                if (!ppustatus.spriteZeroHit && spriteBit12 && bgBit12 && sprite.id == 0 && ppumask.showSprites && ppumask.showBg && dot < 256)
                {
                    ppustatus.val |= 64;
                }

                if (spriteBit12)
                {
                    showSprite = ((bgBit12 && !(sprite.attr & 32)) || !bgBit12) && ppumask.showSprites;
                    spritePaletteIndex = 0x10 | (spritePixel4 << 2) | (spritePixel3 << 2) | spriteBit12;
                    spriteFound = true;
                }

                sprite.shift();
            }
        }

        // When bg rendering is off
        if (!ppumask.showBg)
        {
            paletteIndex = 0;
        }

        uint8_t pindex = NES::PPU::ppuRead(0x3F00 | (showSprite ? spritePaletteIndex : paletteIndex)) % 64;
        // Handling grayscale mode
        uint8_t p = ppumask.greyScale ? (pindex & 0x30) : pindex;

        // Dark border rect to hide seam of scroll, and other glitches that may occur
        if (dot <= 9 || dot >= 249 || scanLine <= 7 || scanLine >= 232)
        {
            showSprite = false;
            p = 13;
        }

        buffer[pixelIndex++] = palette[p];
    }

    inline void NES::PPU::copyHorizontalBits()
    {
        if (isRenderingDisabled())
        {
            return;
        }

        v = (v & ~0x41F) | (t & 0x41F);
    }

    inline void NES::PPU::copyVerticalBits()
    {
        if (isRenderingDisabled())
        {
            return;
        }

        v = (v & ~0x7BE0) | (t & 0x7BE0);
    }

    bool NES::PPU::genNMI()
    {
        if (nmiOccured == true)
        {
            nmiOccured = false;
            return true;
        }
        else
        {
            return false;
        }
    }

    uint8_t NES::PPU::read(uint16_t address)
    {
        address %= 8;

        if (address == 0)
        {
            return ppuctrl.val;
        }
        else if (address == 1)
        {
            return ppumask.val;
        }
        else if (address == 2)
        {
            ppustatus_cpy = ppustatus.val;
            ppustatus.val &= ~0x80;
            w = 0;
            return ppustatus_cpy;
        }
        else if (address == 3)
        {
            return oamaddr;
        }
        else if (address == 4)
        {
            return readOAM(oamaddr);
        }
        else if (address == 7)
        {
            ppu_read_buffer = ppu_read_buffer_cpy;

            if (v >= 0x3F00 && v <= 0x3FFF)
            {
                ppu_read_buffer_cpy = NES::PPU::ppuRead(v - 0x1000);
                ppu_read_buffer = ppumask.greyScale ? (NES::PPU::ppuRead(v) & 0x30) : NES::PPU::ppuRead(v);
            }
            else
            {
                ppu_read_buffer_cpy = NES::PPU::ppuRead(v);
            }

            v += ppuctrl.vramAddressIncrement ? 32 : 1;
            v %= 16384;
            return ppu_read_buffer;
        }

        return 0;
    }

    void NES::PPU::write(uint16_t address, uint8_t data)
    {
        address %= 8;

        if (address == 0)
        {
            t = (t & 0xF3FF) | (((uint16_t)data & 0x03) << 10);
            spriteHeight = (data & 0x20) ? 16 : 8;
            ppuctrl.val = data;
        }
        else if (address == 1)
        {
            ppumask.val = data;
        }
        else if (address == 2)
        {
            data &= ~128;
            ppustatus.val &= 128;
            ppustatus.val |= data;
        }
        else if (address == 3)
        {
            oamaddr = data;
        }
        else if (address == 4 && (scanLine > 239 && scanLine != 241))
        {
            copyOAM(data, oamaddr++);
        }
        else if (address == 5)
        {
            if (w == 0)
            {
                t &= 0x7FE0;
                t |= ((uint16_t)data) >> 3;
                x = data & 7;
                w = 1;
            }
            else
            {
                t &= ~0x73E0;
                t |= ((uint16_t)data & 0x07) << 12;
                t |= ((uint16_t)data & 0xF8) << 2;
                w = 0;
            }

            ppuscroll = data;
        }
        else if (address == 6)
        {
            if (w == 0)
            {
                t &= 255;
                t |= ((uint16_t)data & 0x3F) << 8;
                w = 1;
            }
            else
            {
                t &= 0xFF00;
                t |= data;
                v = t;

                w = 0;
            }
        }
        else if (address == 7)
        {
            NES::PPU::ppuWrite(v, data);
            v += ppuctrl.vramAddressIncrement ? 32 : 1;
            v %= 16384;
        }
    }

    uint8_t NES::PPU::ppuRead(uint16_t address)
    {
        switch (address)
        {
        case 0x0000 ... 0x1FFF:
            return mapper->ppuread(address);
            break;
        case 0x2000 ... 0x2FFF:
            // Horizontal
            if (mapper->getMirroring() == 0)
            {
                if (address >= 0x2400 && address < 0x2800)
                {
                    address -= 0x400;
                }

                if (address >= 0x2800 && address < 0x2c00)
                {
                    address -= 0x400;
                }

                if (address >= 0x2c00 && address < 0x3000)
                {
                    address -= 0x800;
                }
                // Vertical
            }
            else if (mapper->getMirroring() == 1)
            {
                if (address >= 0x2800 && address < 0x3000)
                {
                    address -= 0x800;
                }
                // One-screen mirroring, lower-bank (MMC1)
            }
            else if (mapper->getMirroring() == 2)
            {
                address &= ~0xC00;
                // One-screen mirroring, upper-bank (MMC1)
            }
            else
            {
                address = (address & ~0xC00) + 0x400;
            }

            return vram[address - 0x2000];
            break;
        case 0x3F00 ... 0x3F0F:
            if (address == 0x3F04 || address == 0x3F08 || address == 0x3F0C)
            {
                address = 0x3F00;
            }

            return bg_palette[address - 0x3F00];
            break;
        case 0x3F10 ... 0x3F1F:
            if (address == 0x3F10 || address == 0x3F14 || address == 0x3F18 || address == 0x3F1C)
            {
                return NES::PPU::ppuRead(address & 0x3F0F);
            }
            else
            {
                return sprite_palette[address - 0x3F10];
            }

            return 1;
            break;
        case 0x3000 ... 0x3EFF:
            return NES::PPU::ppuRead(address - 0x1000);
            break;
        default:
            return 1;
            break;
        }
    }

    void NES::PPU::ppuWrite(uint16_t address, uint8_t data)
    {
        switch (address)
        {
        case 0x0000 ... 0x1FFF:
            mapper->ppuwrite(address, data);
            break;
        case 0x2000 ... 0x2FFF:
            // Horizontal
            if (mapper->getMirroring() == 0)
            {
                if (address >= 0x2400 && address < 0x2800)
                {
                    address -= 0x400;
                }

                if (address >= 0x2800 && address < 0x2c00)
                {
                    address -= 0x400;
                }

                if (address >= 0x2c00 && address < 0x3000)
                {
                    address -= 0x800;
                }
                // Vertical
            }
            else if (mapper->getMirroring() == 1)
            {
                if (address >= 0x2800 && address < 0x3000)
                {
                    address -= 0x800;
                }
                // One-screen mirroring, lower-bank (MMC1)
            }
            else if (mapper->getMirroring() == 2)
            {
                address &= ~0xC00;
                // One-screen mirroring, upper-bank (MMC1)
            }
            else
            {
                address = (address & ~0xC00) + 0x400;
            }

            vram[address - 0x2000] = data;
            break;
        case 0x3F00 ... 0x3F0F:
            bg_palette[address - 0x3F00] = data;
            break;
        case 0x3F10 ... 0x3F1F:
            if (address == 0x3F10 || address == 0x3F14 || address == 0x3F18 || address == 0x3F1C)
            {
                bg_palette[(address & 0x3F0F) - 0x3F00] = data;
            }
            else if (address >= 0x3F11 && address <= 0x3F1F)
            {
                sprite_palette[address - 0x3F10] = data;
            }

            break;
        case 0x3000 ... 0x3EFF:
            NES::PPU::ppuWrite(address - 0x1000, data);
            break;
        default:
            break;
        }
    }

    void NES::PPU::copyOAM(uint8_t oamEntry, int index)
    {
        int oamSelect = index / 4;
        int property = index % 4;

        if (property == 0)
        {
            primaryOAM[oamSelect].y = oamEntry;
        }
        else if (property == 1)
        {
            primaryOAM[oamSelect].tileNum = oamEntry;
        }
        else if (property == 2)
        {
            primaryOAM[oamSelect].attr = oamEntry;
        }
        else
        {
            primaryOAM[oamSelect].x = oamEntry;
        }
    }

    uint8_t NES::PPU::readOAM(int index)
    {
        int oamSelect = index / 4;
        int property = index % 4;

        if (property == 0)
        {
            return primaryOAM[oamSelect].y;
        }
        else if (property == 1)
        {
            return primaryOAM[oamSelect].tileNum;
        }
        else if (property == 2)
        {
            return primaryOAM[oamSelect].attr;
        }
        else
        {
            return primaryOAM[oamSelect].x;
        }
    }

    inline void NES::PPU::decrementSpriteCounters()
    {
        if (isRenderingDisabled())
        {
            return;
        }

        for (auto &sprite : spriteRenderEntities)
        {
            if (sprite.counter > 0)
            {
                sprite.counter--;
            }
        }
    }

    bool NES::PPU::isUninit(const Sprite &sprite)
    {
        return ((sprite.attr == 0xFF) && (sprite.tileNum == 0xFF) && (sprite.x == 0xFF) && (sprite.y == 0xFF)) || ((sprite.x == 0) && (sprite.y == 0) && (sprite.attr == 0) && (sprite.tileNum == 0));
    }

    void NES::PPU::evalSprites()
    {
        // clear secondary OAM
        if (dot >= 1 && dot <= 64)
        {
            if (dot == 1)
            {
                secondaryOAMCursor = 0;
            }

            secondaryOAM[secondaryOAMCursor].attr = 0xFF;
            secondaryOAM[secondaryOAMCursor].tileNum = 0xFF;
            secondaryOAM[secondaryOAMCursor].x = 0xFF;
            secondaryOAM[secondaryOAMCursor].y = 0xFF;

            if (dot % 8 == 0)
            {
                secondaryOAMCursor++;
            }
        }

        // sprite eval
        if (dot >= 65 && dot <= 256)
        {
            // Init
            if (dot == 65)
            {
                secondaryOAMCursor = 0;
                primaryOAMCursor = 0;
            }

            if (secondaryOAMCursor == 8)
            {
                // ppustatus |= 0x20;
                return;
            }

            if (primaryOAMCursor == 64)
            {
                return;
            }

            // odd cycle read
            if ((dot % 2) == 1)
            {
                tmpOAM = primaryOAM[primaryOAMCursor];

                if (inYRange(tmpOAM))
                {
                    inRangeCycles--;
                    inRange = true;
                }
                // even cycle write
            }
            else
            {
                // tmpOAM is in range, write it to secondaryOAM
                if (inRange)
                {
                    inRangeCycles--;

                    // copying tmpOAM in range is 8 cycles, 2 cycles otherwise
                    if (inRangeCycles == 0)
                    {
                        primaryOAMCursor++;
                        secondaryOAMCursor++;
                        inRangeCycles = 8;
                        inRange = false;
                    }
                    else
                    {
                        tmpOAM.id = primaryOAMCursor;
                        secondaryOAM[secondaryOAMCursor] = tmpOAM;
                    }
                }
                else
                {
                    primaryOAMCursor++;
                }
            }
        }

        // Sprite fetches
        if (dot >= 257 && dot <= 320)
        {
            if (dot == 257)
            {
                secondaryOAMCursor = 0;
                spriteRenderEntities.clear();
            }

            Sprite sprite = secondaryOAM[secondaryOAMCursor];

            int cycle = (dot - 1) % 8;

            switch (cycle)
            {
            case 0 ... 1:
                if (!isUninit(sprite))
                {
                    out = SpriteRenderEntity();
                }

                break;

            case 2:
                if (!isUninit(sprite))
                {
                    out.attr = sprite.attr;
                    out.flipHorizontally = sprite.attr & 64;
                    out.flipVertically = sprite.attr & 128;
                    out.id = sprite.id;
                }
                break;

            case 3:
                if (!isUninit(sprite))
                {
                    out.counter = sprite.x;
                }
                break;

            case 4:
                if (!isUninit(sprite))
                {
                    spritePatternLowAddr = getSpritePatternAddress(sprite, out.flipVertically);
                    out.lo = NES::PPU::ppuRead(spritePatternLowAddr);
                }
                break;

            case 5:
                break;

            case 6:
                if (!isUninit(sprite))
                {
                    spritePatternHighAddr = spritePatternLowAddr + 8;
                    out.hi = NES::PPU::ppuRead(spritePatternHighAddr);
                }
                break;

            case 7:
                if (!isUninit(sprite))
                {
                    spriteRenderEntities.push_back(out);
                }

                secondaryOAMCursor++;
                break;

            default:
                break;
            }
        }
    }

    uint16_t NES::PPU::getSpritePatternAddress(const Sprite &sprite, bool flipVertically)
    {
        uint16_t addr = 0;

        int fineOffset = scanLine - sprite.y;

        if (flipVertically)
        {
            fineOffset = spriteHeight - 1 - fineOffset;
        }

        // By adding 8 to fineOffset we skip the high order bits
        if (spriteHeight == 16 && fineOffset >= 8)
        {
            fineOffset += 8;
        }

        if (spriteHeight == 8)
        {
            addr = ((uint16_t)ppuctrl.spritePatternTableAddress << 12) |
                   ((uint16_t)sprite.tileNum << 4) |
                   fineOffset;
        }
        else
        {
            addr = (((uint16_t)sprite.tileNum & 1) << 12) |
                   ((uint16_t)((sprite.tileNum & ~1) << 4)) |
                   fineOffset;
        }

        return addr;
    }

    bool NES::PPU::inYRange(const Sprite &oam)
    {
        return !isUninit(oam) && ((scanLine >= oam.y) && (scanLine < (oam.y + spriteHeight)));
    }

    void NES::PPU::printState()
    {
        std::cout << "scanline=" << unsigned(scanLine) << ", " << unsigned(dot) << std::endl
                  << std::endl;
    }
} // namespace NES
