#include "Chip8Processor.h"
#include <thread>
namespace chip8
{

Chip8Processor::Chip8Processor()
{
    Reset();
    _runThread = NULL;
}

Chip8Processor::~Chip8Processor()
{
}

bool Chip8Processor::LoadRom(const uint8_t* src, uint16_t length)
{
    if (length > (Chip8Processor::RAM_SIZE - Chip8Processor::ROM_OFFSET))
    {
        return false;
    }

    for (uint16_t i = 0; i < length; i++)
    {
        _RAM[i + Chip8Processor::ROM_OFFSET] = src[i];
    }
    return true;
}

bool Chip8Processor::Reset()
{
    Stop();

    _v0 = 0;
    _v1 = 0;
    _v2 = 0;
    _v3 = 0;
    _v4 = 0;
    _v5 = 0;
    _v6 = 0;
    _v7 = 0;
    _v8 = 0;
    _v9 = 0;
    _vA = 0;
    _vB = 0;
    _vC = 0;
    _vD = 0;
    _vE = 0;
    _vF = 0;
    _I = 0;
    _pc = Chip8Processor::ROM_OFFSET;
    _sp = Chip8Processor::RAM_SIZE - 2;
    _delayTimer = 0;
    _soundTimer = 0;

    //TODO: Reset counters
    return true;
}

bool Chip8Processor::Run()
{
    _runLock.lock();
    if (!_run)
    {
        _runThread = new std::thread(&Chip8Processor::ExecutionThread, this);
        _run = true;
    }
    _runLock.unlock();
    return _run;
}

bool Chip8Processor::Stop()
{
    _runLock.lock();
    if (_run)
    {
        _run = false;
        _runThread->join();
        delete _runThread;
        _runThread = NULL;
        _runLock.unlock();
    }
    return _run;
}

bool Chip8Processor::IsRunning()
{
    return _run;
}

void Chip8Processor::ExecutionThread()
{
    while(_run)
    {
        uint16_t instruction = _RAM[_pc];
        HandleInstruction(instruction);
    }
    return;
}

bool Chip8Processor::HandleInstruction(uint16_t instruction)
{

}
}
