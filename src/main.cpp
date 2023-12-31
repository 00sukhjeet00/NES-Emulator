#include <iostream>
#include <SDL2/SDL.h>
#include <map>

#include "../include/6502.hpp"
#include "../include/Controller.hpp"
#include "../include/Mapper/Mapper.hpp"
#include "../include/PPU.hpp"
#include "../include/ROM.hpp"

int main(int argc, char **argv)
{
    std::string romPath = "";
    std::string COMMAND_LINE_ERROR_MESSAGE = "Use -insert <path/to/rom> to start playing.";
    bool fullScreen = false;

    if (argc < 2)
    {
        std::cout << COMMAND_LINE_ERROR_MESSAGE << "\n";
    }
    std::string option = argv[1];

    if (option == "-insert")
    {
        romPath = argv[2];
    }
    else
    {
        std::cout << "Unknow Option\n";
    }

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0)
    {
        std::cout << "SDL not working" << SDL_GetError() << "\n";
    }
    SDL_GameController *controller = nullptr;

    for (int i = 0; i < SDL_NumJoysticks(); i++)
    {
        controller = SDL_GameControllerOpen(i);
        std::cout << "Controller detected.";
        break;
    }

    std::map<int, int> map;
    map.insert(std::pair<int, int>(SDL_CONTROLLER_BUTTON_A, SDLK_a));
    map.insert(std::pair<int, int>(SDL_CONTROLLER_BUTTON_B, SDLK_b));
    map.insert(std::pair<int, int>(SDL_CONTROLLER_BUTTON_START, SDLK_RETURN));
    map.insert(std::pair<int, int>(SDL_CONTROLLER_BUTTON_DPAD_UP, SDLK_UP));
    map.insert(std::pair<int, int>(SDL_CONTROLLER_BUTTON_DPAD_DOWN, SDLK_DOWN));
    map.insert(std::pair<int, int>(SDL_CONTROLLER_BUTTON_DPAD_LEFT, SDLK_LEFT));
    map.insert(std::pair<int, int>(SDL_CONTROLLER_BUTTON_DPAD_RIGHT, SDLK_RIGHT));

    SDL_Window *window;
    std::string windowTitle = "NES Emulator";
    bool headLessMode = false;

    window = SDL_CreateWindow(windowTitle.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 512, 480, SDL_WINDOW_SHOWN);

    if (window == NULL)
    {
        std::cout << "Could not create window: " << SDL_GetError() << "\n";
        return 1;
    }

    if (fullScreen)
    {
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
    }

    bool isRunning = true;
    SDL_Event event;
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!renderer)
    {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    NES::ROM rom;
    rom.open(romPath);
    rom.printHeader();
    NES::Mapper *mapper = rom.getMapper();

    if (mapper == NULL)
    {
        std::cout << "Unkown Mapper";
        return -1;
    }
    auto ppu = NES::PPU(mapper);
    NES::Controller Controller;
    auto cpu = NES::CPU6502(mapper, &ppu, &Controller);
    cpu.reset();
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, 256, 240);

    while (isRunning)
    {
        cpu.step();
        if (ppu.generateFrame)
        {
            while (SDL_PollEvent(&event))
            {
                switch (event.type)
                {
                case SDL_CONTROLLERBUTTONDOWN:
                    Controller.setButtonPressed(map.find(event.cbutton.button)->second, true);
                    break;
                case SDL_CONTROLLERBUTTONUP:
                    Controller.setButtonPressed(map.find(event.cbutton.button)->second, false);
                    break;
                case SDL_KEYDOWN:
                    Controller.setButtonPressed(event.key.keysym.sym, true);
                    break;
                case SDL_KEYUP:
                    Controller.setButtonPressed(event.key.keysym.sym, false);
                    break;
                case SDL_QUIT:
                    isRunning = false;
                    break;
                default:
                    break;
                }
            }
            ppu.generateFrame = false;
            SDL_RenderSetScale(renderer, 2, 2);
            SDL_UpdateTexture(texture, NULL, ppu.buffer, 256 * sizeof(Uint32));
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, NULL, NULL);
            SDL_RenderPresent(renderer);
        }
    }

    SDL_Delay(3000);

    SDL_DestroyWindow(window);

    return 0;
}