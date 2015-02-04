#include "Chip8Processor.h"
#include "Keyboard.h"
#include "Display.h"
#include "Beeper.h"
#include <iostream>
#include <fstream>


#define LOG_TAG "main"
#include "log.h"

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        LOG("You must specify a file!");
        exit(-1);
    }
    char* romPath = argv[1];
    LOG("Loading %s", romPath);
    chip8::Display* disp = new chip8::Display();
    LOG("Creating keyboard");
    chip8::Keyboard* kb = new chip8::Keyboard();
    LOG("Creating beeper");
    chip8::Beeper* beeper = new chip8::Beeper();
    LOG("Creating processor");
    chip8::Chip8Processor* proc = new chip8::Chip8Processor(kb, disp, beeper);

    std::ifstream romFile(romPath, std::ifstream::binary);
    uint8_t buffer[3584] = {0};  // Max file size
    romFile.read((char*)buffer, 3584);
    LOG("Loading rom");
    proc->LoadRom(buffer, 3584);
    LOG("Resetting processor");
    proc->Reset();
    LOG("Run!");
    proc->Run();
    while (proc->IsRunning())
    {
       std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
