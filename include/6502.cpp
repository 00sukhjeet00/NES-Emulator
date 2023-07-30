#include "6502.hpp"
namespace NES
{
    void NES::CPU6502::step()
    {
        if ()
        {
        }
        uint8_t instruction = fetchInstruction();
        executeInstruction(instruction);
        programCounter++;
    }
} // namespace NES
