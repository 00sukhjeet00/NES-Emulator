#pragma once

#include <SDL2/SDL.h>
#include <stdio.h>
#include "Bus.hpp"
#include <string>

namespace NES
{
    class Controller
    {
        uint8_t JOY1 = 0;
        uint8_t JOY2 = 0;
        uint8_t btnStateLocked = 0;
        uint8_t btnState = 0;
        bool strobe;

    public:
        // Bus
        uint8_t read(uint16_t address);
        void write(uint16_t address, uint8_t data);

        // Input
        void setButtonPressed(SDL_Keycode, bool);
    };

} // namespace NES
