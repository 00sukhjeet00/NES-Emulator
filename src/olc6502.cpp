#include "../include/olc6502.hpp"
#include "../include/Bus.hpp"

olc6502::olc6502()
{
    // It is 16x16 entries. This gives 256 instructions. It is arranged to that the bottom
    // 4 bits of the instruction choose the column, and the top 4 bits choose the row.

    using a = olc6502;
    lookup =
        {
            {"BRK", &a::BRK, &a::IMM, 7},
            {"ORA", &a::ORA, &a::IZX, 6},
            {"???", &a::XXX, &a::IMP, 2},
            {"???", &a::XXX, &a::IMP, 8},
            {"???", &a::NOP, &a::IMP, 3},
            {"ORA", &a::ORA, &a::ZP0, 3},
            {"ASL", &a::ASL, &a::ZP0, 5},
            {"???", &a::XXX, &a::IMP, 5},
            {"PHP", &a::PHP, &a::IMP, 3},
            {"ORA", &a::ORA, &a::IMM, 2},
            {"ASL", &a::ASL, &a::IMP, 2},
            {"???", &a::XXX, &a::IMP, 2},
            {"???", &a::NOP, &a::IMP, 4},
            {"ORA", &a::ORA, &a::ABS, 4},
            {"ASL", &a::ASL, &a::ABS, 6},
            {"???", &a::XXX, &a::IMP, 6},
            {"BPL", &a::BPL, &a::REL, 2},
            {"ORA", &a::ORA, &a::IZY, 5},
            {"???", &a::XXX, &a::IMP, 2},
            {"???", &a::XXX, &a::IMP, 8},
            {"???", &a::NOP, &a::IMP, 4},
            {"ORA", &a::ORA, &a::ZPX, 4},
            {"ASL", &a::ASL, &a::ZPX, 6},
            {"???", &a::XXX, &a::IMP, 6},
            {"CLC", &a::CLC, &a::IMP, 2},
            {"ORA", &a::ORA, &a::ABY, 4},
            {"???", &a::NOP, &a::IMP, 2},
            {"???", &a::XXX, &a::IMP, 7},
            {"???", &a::NOP, &a::IMP, 4},
            {"ORA", &a::ORA, &a::ABX, 4},
            {"ASL", &a::ASL, &a::ABX, 7},
            {"???", &a::XXX, &a::IMP, 7},
            {"JSR", &a::JSR, &a::ABS, 6},
            {"AND", &a::AND, &a::IZX, 6},
            {"???", &a::XXX, &a::IMP, 2},
            {"???", &a::XXX, &a::IMP, 8},
            {"BIT", &a::BIT, &a::ZP0, 3},
            {"AND", &a::AND, &a::ZP0, 3},
            {"ROL", &a::ROL, &a::ZP0, 5},
            {"???", &a::XXX, &a::IMP, 5},
            {"PLP", &a::PLP, &a::IMP, 4},
            {"AND", &a::AND, &a::IMM, 2},
            {"ROL", &a::ROL, &a::IMP, 2},
            {"???", &a::XXX, &a::IMP, 2},
            {"BIT", &a::BIT, &a::ABS, 4},
            {"AND", &a::AND, &a::ABS, 4},
            {"ROL", &a::ROL, &a::ABS, 6},
            {"???", &a::XXX, &a::IMP, 6},
            {"BMI", &a::BMI, &a::REL, 2},
            {"AND", &a::AND, &a::IZY, 5},
            {"???", &a::XXX, &a::IMP, 2},
            {"???", &a::XXX, &a::IMP, 8},
            {"???", &a::NOP, &a::IMP, 4},
            {"AND", &a::AND, &a::ZPX, 4},
            {"ROL", &a::ROL, &a::ZPX, 6},
            {"???", &a::XXX, &a::IMP, 6},
            {"SEC", &a::SEC, &a::IMP, 2},
            {"AND", &a::AND, &a::ABY, 4},
            {"???", &a::NOP, &a::IMP, 2},
            {"???", &a::XXX, &a::IMP, 7},
            {"???", &a::NOP, &a::IMP, 4},
            {"AND", &a::AND, &a::ABX, 4},
            {"ROL", &a::ROL, &a::ABX, 7},
            {"???", &a::XXX, &a::IMP, 7},
            {"RTI", &a::RTI, &a::IMP, 6},
            {"EOR", &a::EOR, &a::IZX, 6},
            {"???", &a::XXX, &a::IMP, 2},
            {"???", &a::XXX, &a::IMP, 8},
            {"???", &a::NOP, &a::IMP, 3},
            {"EOR", &a::EOR, &a::ZP0, 3},
            {"LSR", &a::LSR, &a::ZP0, 5},
            {"???", &a::XXX, &a::IMP, 5},
            {"PHA", &a::PHA, &a::IMP, 3},
            {"EOR", &a::EOR, &a::IMM, 2},
            {"LSR", &a::LSR, &a::IMP, 2},
            {"???", &a::XXX, &a::IMP, 2},
            {"JMP", &a::JMP, &a::ABS, 3},
            {"EOR", &a::EOR, &a::ABS, 4},
            {"LSR", &a::LSR, &a::ABS, 6},
            {"???", &a::XXX, &a::IMP, 6},
            {"BVC", &a::BVC, &a::REL, 2},
            {"EOR", &a::EOR, &a::IZY, 5},
            {"???", &a::XXX, &a::IMP, 2},
            {"???", &a::XXX, &a::IMP, 8},
            {"???", &a::NOP, &a::IMP, 4},
            {"EOR", &a::EOR, &a::ZPX, 4},
            {"LSR", &a::LSR, &a::ZPX, 6},
            {"???", &a::XXX, &a::IMP, 6},
            {"CLI", &a::CLI, &a::IMP, 2},
            {"EOR", &a::EOR, &a::ABY, 4},
            {"???", &a::NOP, &a::IMP, 2},
            {"???", &a::XXX, &a::IMP, 7},
            {"???", &a::NOP, &a::IMP, 4},
            {"EOR", &a::EOR, &a::ABX, 4},
            {"LSR", &a::LSR, &a::ABX, 7},
            {"???", &a::XXX, &a::IMP, 7},
            {"RTS", &a::RTS, &a::IMP, 6},
            {"ADC", &a::ADC, &a::IZX, 6},
            {"???", &a::XXX, &a::IMP, 2},
            {"???", &a::XXX, &a::IMP, 8},
            {"???", &a::NOP, &a::IMP, 3},
            {"ADC", &a::ADC, &a::ZP0, 3},
            {"ROR", &a::ROR, &a::ZP0, 5},
            {"???", &a::XXX, &a::IMP, 5},
            {"PLA", &a::PLA, &a::IMP, 4},
            {"ADC", &a::ADC, &a::IMM, 2},
            {"ROR", &a::ROR, &a::IMP, 2},
            {"???", &a::XXX, &a::IMP, 2},
            {"JMP", &a::JMP, &a::IND, 5},
            {"ADC", &a::ADC, &a::ABS, 4},
            {"ROR", &a::ROR, &a::ABS, 6},
            {"???", &a::XXX, &a::IMP, 6},
            {"BVS", &a::BVS, &a::REL, 2},
            {"ADC", &a::ADC, &a::IZY, 5},
            {"???", &a::XXX, &a::IMP, 2},
            {"???", &a::XXX, &a::IMP, 8},
            {"???", &a::NOP, &a::IMP, 4},
            {"ADC", &a::ADC, &a::ZPX, 4},
            {"ROR", &a::ROR, &a::ZPX, 6},
            {"???", &a::XXX, &a::IMP, 6},
            {"SEI", &a::SEI, &a::IMP, 2},
            {"ADC", &a::ADC, &a::ABY, 4},
            {"???", &a::NOP, &a::IMP, 2},
            {"???", &a::XXX, &a::IMP, 7},
            {"???", &a::NOP, &a::IMP, 4},
            {"ADC", &a::ADC, &a::ABX, 4},
            {"ROR", &a::ROR, &a::ABX, 7},
            {"???", &a::XXX, &a::IMP, 7},
            {"???", &a::NOP, &a::IMP, 2},
            {"STA", &a::STA, &a::IZX, 6},
            {"???", &a::NOP, &a::IMP, 2},
            {"???", &a::XXX, &a::IMP, 6},
            {"STY", &a::STY, &a::ZP0, 3},
            {"STA", &a::STA, &a::ZP0, 3},
            {"STX", &a::STX, &a::ZP0, 3},
            {"???", &a::XXX, &a::IMP, 3},
            {"DEY", &a::DEY, &a::IMP, 2},
            {"???", &a::NOP, &a::IMP, 2},
            {"TXA", &a::TXA, &a::IMP, 2},
            {"???", &a::XXX, &a::IMP, 2},
            {"STY", &a::STY, &a::ABS, 4},
            {"STA", &a::STA, &a::ABS, 4},
            {"STX", &a::STX, &a::ABS, 4},
            {"???", &a::XXX, &a::IMP, 4},
            {"BCC", &a::BCC, &a::REL, 2},
            {"STA", &a::STA, &a::IZY, 6},
            {"???", &a::XXX, &a::IMP, 2},
            {"???", &a::XXX, &a::IMP, 6},
            {"STY", &a::STY, &a::ZPX, 4},
            {"STA", &a::STA, &a::ZPX, 4},
            {"STX", &a::STX, &a::ZPY, 4},
            {"???", &a::XXX, &a::IMP, 4},
            {"TYA", &a::TYA, &a::IMP, 2},
            {"STA", &a::STA, &a::ABY, 5},
            {"TXS", &a::TXS, &a::IMP, 2},
            {"???", &a::XXX, &a::IMP, 5},
            {"???", &a::NOP, &a::IMP, 5},
            {"STA", &a::STA, &a::ABX, 5},
            {"???", &a::XXX, &a::IMP, 5},
            {"???", &a::XXX, &a::IMP, 5},
            {"LDY", &a::LDY, &a::IMM, 2},
            {"LDA", &a::LDA, &a::IZX, 6},
            {"LDX", &a::LDX, &a::IMM, 2},
            {"???", &a::XXX, &a::IMP, 6},
            {"LDY", &a::LDY, &a::ZP0, 3},
            {"LDA", &a::LDA, &a::ZP0, 3},
            {"LDX", &a::LDX, &a::ZP0, 3},
            {"???", &a::XXX, &a::IMP, 3},
            {"TAY", &a::TAY, &a::IMP, 2},
            {"LDA", &a::LDA, &a::IMM, 2},
            {"TAX", &a::TAX, &a::IMP, 2},
            {"???", &a::XXX, &a::IMP, 2},
            {"LDY", &a::LDY, &a::ABS, 4},
            {"LDA", &a::LDA, &a::ABS, 4},
            {"LDX", &a::LDX, &a::ABS, 4},
            {"???", &a::XXX, &a::IMP, 4},
            {"BCS", &a::BCS, &a::REL, 2},
            {"LDA", &a::LDA, &a::IZY, 5},
            {"???", &a::XXX, &a::IMP, 2},
            {"???", &a::XXX, &a::IMP, 5},
            {"LDY", &a::LDY, &a::ZPX, 4},
            {"LDA", &a::LDA, &a::ZPX, 4},
            {"LDX", &a::LDX, &a::ZPY, 4},
            {"???", &a::XXX, &a::IMP, 4},
            {"CLV", &a::CLV, &a::IMP, 2},
            {"LDA", &a::LDA, &a::ABY, 4},
            {"TSX", &a::TSX, &a::IMP, 2},
            {"???", &a::XXX, &a::IMP, 4},
            {"LDY", &a::LDY, &a::ABX, 4},
            {"LDA", &a::LDA, &a::ABX, 4},
            {"LDX", &a::LDX, &a::ABY, 4},
            {"???", &a::XXX, &a::IMP, 4},
            {"CPY", &a::CPY, &a::IMM, 2},
            {"CMP", &a::CMP, &a::IZX, 6},
            {"???", &a::NOP, &a::IMP, 2},
            {"???", &a::XXX, &a::IMP, 8},
            {"CPY", &a::CPY, &a::ZP0, 3},
            {"CMP", &a::CMP, &a::ZP0, 3},
            {"DEC", &a::DEC, &a::ZP0, 5},
            {"???", &a::XXX, &a::IMP, 5},
            {"INY", &a::INY, &a::IMP, 2},
            {"CMP", &a::CMP, &a::IMM, 2},
            {"DEX", &a::DEX, &a::IMP, 2},
            {"???", &a::XXX, &a::IMP, 2},
            {"CPY", &a::CPY, &a::ABS, 4},
            {"CMP", &a::CMP, &a::ABS, 4},
            {"DEC", &a::DEC, &a::ABS, 6},
            {"???", &a::XXX, &a::IMP, 6},
            {"BNE", &a::BNE, &a::REL, 2},
            {"CMP", &a::CMP, &a::IZY, 5},
            {"???", &a::XXX, &a::IMP, 2},
            {"???", &a::XXX, &a::IMP, 8},
            {"???", &a::NOP, &a::IMP, 4},
            {"CMP", &a::CMP, &a::ZPX, 4},
            {"DEC", &a::DEC, &a::ZPX, 6},
            {"???", &a::XXX, &a::IMP, 6},
            {"CLD", &a::CLD, &a::IMP, 2},
            {"CMP", &a::CMP, &a::ABY, 4},
            {"NOP", &a::NOP, &a::IMP, 2},
            {"???", &a::XXX, &a::IMP, 7},
            {"???", &a::NOP, &a::IMP, 4},
            {"CMP", &a::CMP, &a::ABX, 4},
            {"DEC", &a::DEC, &a::ABX, 7},
            {"???", &a::XXX, &a::IMP, 7},
            {"CPX", &a::CPX, &a::IMM, 2},
            {"SBC", &a::SBC, &a::IZX, 6},
            {"???", &a::NOP, &a::IMP, 2},
            {"???", &a::XXX, &a::IMP, 8},
            {"CPX", &a::CPX, &a::ZP0, 3},
            {"SBC", &a::SBC, &a::ZP0, 3},
            {"INC", &a::INC, &a::ZP0, 5},
            {"???", &a::XXX, &a::IMP, 5},
            {"INX", &a::INX, &a::IMP, 2},
            {"SBC", &a::SBC, &a::IMM, 2},
            {"NOP", &a::NOP, &a::IMP, 2},
            {"???", &a::SBC, &a::IMP, 2},
            {"CPX", &a::CPX, &a::ABS, 4},
            {"SBC", &a::SBC, &a::ABS, 4},
            {"INC", &a::INC, &a::ABS, 6},
            {"???", &a::XXX, &a::IMP, 6},
            {"BEQ", &a::BEQ, &a::REL, 2},
            {"SBC", &a::SBC, &a::IZY, 5},
            {"???", &a::XXX, &a::IMP, 2},
            {"???", &a::XXX, &a::IMP, 8},
            {"???", &a::NOP, &a::IMP, 4},
            {"SBC", &a::SBC, &a::ZPX, 4},
            {"INC", &a::INC, &a::ZPX, 6},
            {"???", &a::XXX, &a::IMP, 6},
            {"SED", &a::SED, &a::IMP, 2},
            {"SBC", &a::SBC, &a::ABY, 4},
            {"NOP", &a::NOP, &a::IMP, 2},
            {"???", &a::XXX, &a::IMP, 7},
            {"???", &a::NOP, &a::IMP, 4},
            {"SBC", &a::SBC, &a::ABX, 4},
            {"INC", &a::INC, &a::ABX, 7},
            {"???", &a::XXX, &a::IMP, 7},
        };
}

void olc6502::write(uint16_t addr, uint8_t data)
{
    bus->write(addr, data);
}
uint8_t olc6502::read(uint16_t addr)
{
    return bus->read(addr, false);
}

void olc6502::clock()
{
    if (cycles == 0)
    {
        opcode = read(programCounter);
        programCounter++;
        cycles = lookup[opcode].cycles;
        uint8_t additional_cycle1 = (this->*lookup[opcode].addrmode)();
        uint8_t additional_cycle2 = (this->*lookup[opcode].operate)();

        cycles += (additional_cycle1 & additional_cycle2);
    }
    cycles--;
}

uint8_t olc6502::getFlag(olc6502::FLAG6502 flag)
{
    return ((status & flag) > 0) ? 1 : 0;
}

void olc6502::setFlag(olc6502::FLAG6502 flag, bool value)
{
    if (value)
        status |= flag;
    else
        status &= ~flag;
}

uint8_t olc6502::IMP()
{
    fetched = accumulator;
    return 0;
}

// Address Mode: Immediate
// The instruction expects the next byte to be used as a value, so we'll prep
// the read address to point to the next byte
uint8_t olc6502::IMM()
{
    addr_abs = programCounter++;
    return 0;
}

// Address Mode: Zero Page
// To save program bytes, zero page addressing allows you to absolutely address
// a location in first 0xFF bytes of address range. Clearly this only requires
// one byte instead of the usual two.
uint8_t olc6502::ZP0()
{
    addr_abs = read(programCounter);
    programCounter++;
    addr_abs &= 0x00FF;
    return 0;
}

// Address Mode: Zero Page with X Offset
// Fundamentally the same as Zero Page addressing, but the contents of the X Register
// is added to the supplied single byte address. This is useful for iterating through
// ranges within the first page.
uint8_t olc6502::ZPX()
{
    addr_abs = (read(programCounter) + xRegister);
    programCounter++;
    addr_abs &= 0x00FF;
    return 0;
}

uint8_t olc6502::ZPY()
{
    addr_abs = (read(programCounter) + yRegister);
    programCounter++;
    addr_abs &= 0x00FF;
    return 0;
}

uint8_t olc6502::REL()
{
    addr_rel = read(programCounter);
    programCounter++;
    if (addr_rel & 0x80)
        addr_rel |= 0xFF00;
    return 0;
}

// Address Mode: Absolute
// A full 16-bit address is loaded and used
uint8_t olc6502::ABS()
{
    uint16_t lo = read(programCounter);
    programCounter++;
    uint16_t hi = read(programCounter);
    programCounter++;

    addr_abs = (hi << 8) | lo;

    return 0;
}

// Address Mode: Absolute with X Offset
// Fundamentally the same as absolute addressing, but the contents of the X Register
// is added to the supplied two byte address. If the resulting address changes
// the page, an additional clock cycle is required
uint8_t olc6502::ABX()
{
    uint16_t lo = read(programCounter);
    programCounter++;
    uint16_t hi = read(programCounter);
    programCounter++;

    addr_abs = (hi << 8) | lo;
    addr_abs += xRegister;

    if ((addr_abs & 0xFF00) != (hi << 8))
        return 1;
    else
        return 0;
}

// Address Mode: Absolute with Y Offset
// Fundamentally the same as absolute addressing, but the contents of the Y Register
// is added to the supplied two byte address. If the resulting address changes
// the page, an additional clock cycle is required
uint8_t olc6502::ABY()
{
    uint16_t lo = read(programCounter);
    programCounter++;
    uint16_t hi = read(programCounter);
    programCounter++;

    addr_abs = (hi << 8) | lo;
    addr_abs += yRegister;

    if ((addr_abs & 0xFF00) != (hi << 8))
        return 1;
    else
        return 0;
}

// Note: The next 3 address modes use indirection (aka Pointers!)

// Address Mode: Indirect
// The supplied 16-bit address is read to get the actual 16-bit address. This is
// instruction is unusual in that it has a bug in the hardware! To emulate its
// function accurately, we also need to emulate this bug. If the low byte of the
// supplied address is 0xFF, then to read the high byte of the actual address
// we need to cross a page boundary. This doesnt actually work on the chip as
// designed, instead it wraps back around in the same page, yielding an
// invalid actual address
uint8_t olc6502::IND()
{
    uint16_t ptr_lo = read(programCounter);
    programCounter++;
    uint16_t ptr_hi = read(programCounter);
    programCounter++;

    uint16_t ptr = (ptr_hi << 8) | ptr_lo;

    if (ptr_lo == 0x00FF) // Simulate page boundary hardware bug
    {
        addr_abs = (read(ptr & 0xFF00) << 8) | read(ptr + 0);
    }
    else // Behave normally
    {
        addr_abs = (read(ptr + 1) << 8) | read(ptr + 0);
    }

    return 0;
}

// Address Mode: Indirect X
// The supplied 8-bit address is offset by X Register to index
// a location in page 0x00. The actual 16-bit address is read
// from this location
uint8_t olc6502::IZX()
{
    uint16_t t = read(programCounter);
    programCounter++;

    uint16_t lo = read((uint16_t)(t + (uint16_t)xRegister) & 0x00FF);
    uint16_t hi = read((uint16_t)(t + (uint16_t)xRegister + 1) & 0x00FF);

    addr_abs = (hi << 8) | lo;

    return 0;
}

// Address Mode: Indirect Y
// The supplied 8-bit address indexes a location in page 0x00. From
// here the actual 16-bit address is read, and the contents of
// Y Register is added to it to offset it. If the offset causes a
// change in page then an additional clock cycle is required.
uint8_t olc6502::IZY()
{
    uint16_t t = read(programCounter);
    programCounter++;

    uint16_t lo = read(t & 0x00FF);
    uint16_t hi = read((t + 1) & 0x00FF);

    addr_abs = (hi << 8) | lo;
    addr_abs += yRegister;

    if ((addr_abs & 0xFF00) != (hi << 8))
        return 1;
    else
        return 0;
}
