#include "Chip8Processor.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define LOG(...) fprintf(stderr, __VA_ARGS__)

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

    memset(_v, sizeof(_v), 0);
    _I = 0;
    _pc = Chip8Processor::ROM_OFFSET;
    _sp = Chip8Processor::STACK_SIZE;
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
        LOG("pc = %4x", _pc);
        HandleInstruction(instruction);
    }
    return;
}

bool Chip8Processor::HandleInstruction(uint16_t instruction)
{
    LOG("%s", __FUNCTION__);
    uint8_t firstNibble = instruction & 0xF000;

    uint8_t xRegister = (instruction & 0x0F00) >> 8;
    uint8_t yRegister = (instruction & 0x00F0) >> 4;

    switch (firstNibble)
    {
        case 0:
        {
            if (instruction == 0x00E0)
            {
                return ClearScreen();
            }
            else if (instruction == 0x0EE)
            {
                return Return();
            }
        }
        break;

        case 1:
        case 11:
        {
            uint8_t offset = (firstNibble == 1) ? 0 : _v[0];
            uint16_t address = (instruction & 0x0FFF) + offset;
            return Jump(address);
        }
        break;

        case 2:
        {
            return Call(instruction & 0x0FFF);
        }
        break;

        case 3:
        case 4:
        {
            uint8_t value = (instruction & 0x00FF);
            return SkipValue(xRegister, value, (firstNibble == 3));
        }
        break;

        case 5:
        case 9:
        {
            if ((instruction & 0x000F) != 0)
            {
                return false;
            }
            return SkipXY(xRegister, yRegister, (firstNibble == 5));
        }
        break;

        case 6:
        {
            uint8_t value = (instruction & 0x00FF);
            return SetByValue(xRegister, value);
        }
        break;

        case 7:
        {
            uint8_t value = (instruction & 0x00FF);
            return AddToRegister(xRegister, value);
        }
        break;

        case 8:
        {
            Chip8Processor::MathCode code = (Chip8Processor::MathCode)(instruction & 0x000F);
            return Math(xRegister, yRegister, code);
        }
        break;

        case 10:
        {
            return SetIRegister(instruction & 0x0FFF);
        }
        break;

        case 12:
        {
            uint8_t mask = (instruction & 0x00FF);
            return SetRandom(xRegister, mask);
        }
        break;

        case 13:
        {
            uint8_t size = (instruction & 0x000F);
            return DrawSprite(xRegister, yRegister, size);
        }
        break;

        default:
        {
            switch (instruction & 0xF0FF)
            {
                case 0xE09E:
                case 0xE0A1:
                {
                    return SkipKeyPress(xRegister, (instruction & 0xF0FF) == 0xE09E);
                }
                break;

                case 0xF007:
                {
                    return StoreDelayTimer(xRegister);
                }
                break;

                case 0xF00A:
                {
                    return WaitAndStoreKey(xRegister);
                }
                break;

                case 0xF015:
                {
                    return SetDelayTimer(xRegister);
                }
                break;

                case 0xF018:
                {
                    return SetSoundTimer(xRegister);
                }
                break;

                case 0xF01E:
                {
                    return AddToI(xRegister);
                }
                break;

                case 0xF029:
                {
                    return SetIToChar(xRegister);
                }
                break;

                case 0xF033:
                {
                    return StoreBCD(xRegister);
                }
                break;

                case 0xF055:
                {
                    return StoreRegs(xRegister);
                }
                break;

                case 0xF065:
                {
                    return FillRegs(xRegister);
                }
                break;
            }
        }
        break;
    }
    LOG("No instruction handled");
    return false;
}

// Instructions
bool Chip8Processor::ClearScreen()
{
    //TODO:  Implement me
    return false;
}

bool Chip8Processor::Return()
{
    _pc = _stack[_sp];
    _sp++;
    return (_sp >= Chip8Processor::STACK_SIZE);
}

bool Chip8Processor::Jump(uint16_t address)
{
    _pc = address;
    return true;
}

bool Chip8Processor::Call(uint16_t address)
{
    _sp--;
    _stack[_sp] = _pc + 2;
    _pc = address;
    return (_sp >= 0);
}

bool Chip8Processor::SkipValue(uint8_t xRegister, uint8_t value, bool ifEqual)
{
    _pc += 2;
    bool isEqual = (_v[xRegister] == value);
    if (isEqual == ifEqual)
    {
        _pc += 2;
    }
    return true;
}

bool Chip8Processor::SkipXY(uint8_t xRegister, uint8_t yRegister, bool ifEqual)
{
    _pc += 2;
    bool isEqual = (_v[xRegister] == _v[yRegister]);
    if (isEqual == ifEqual)
    {
        _pc += 2;
    }
    return true;
}

bool Chip8Processor::SetByValue(uint8_t xRegister, uint8_t value)
{
    _pc += 2;
    _v[xRegister] = value;
    return true;
}

bool Chip8Processor::AddToRegister(uint8_t xRegister, uint8_t value)
{
    _pc += 2;
    _v[xRegister] = value;
    return true;
}

bool Chip8Processor::Math(uint8_t xRegister, uint8_t yRegister, MathCode code)
{
    _pc += 2;
    switch (code)
    {
        case Chip8Processor::MATH_ADD:
        {
            uint8_t oldX = _v[xRegister];
            _v[xRegister] += _v[yRegister];
            _v[15] = (_v[xRegister] < oldX);
        }
        break;

        case Chip8Processor::MATH_AND:
        {
            _v[xRegister] &= _v[yRegister];
        }
        break;

        case Chip8Processor::MATH_MINUS:
        {
            _v[15] = (_v[yRegister] > _v[xRegister]) ? 1 : 0;
            _v[xRegister] = _v[yRegister] - _v[xRegister];
        }
        break;


        case Chip8Processor::MATH_OR:
        {
            _v[xRegister] |= _v[yRegister];
        }
        break;

        case Chip8Processor::MATH_SET:
        {
            _v[xRegister] = _v[yRegister];
        }
        break;

        case Chip8Processor::MATH_SL:
        {
            _v[15] = (_v[xRegister] & 0x80) == 0 ? 0 : 1;
            _v[xRegister] <<= 1;
        }
        break;

        case Chip8Processor::MATH_SR:
        {
            _v[15] = (_v[xRegister] & 0x01) == 0 ? 0 : 1;
            _v[xRegister] >>= 1;
        }
        break;

        case Chip8Processor::MATH_SUB:
        {
            _v[15] = (v[xRegister] > _v[yRegister]) ? 1 : 0;
            _v[xRegister] = _v[xRegister] - _v[yRegister];
        }
        break;

        case Chip8Processor::MATH_XOR:
        {
            _v[xRegister] ^= _v[yRegister];
        }
        break;

        default:
        {
            return false;
        }
    }
    return true;
}

bool Chip8Processor::SetIRegister(uint16_t value)
{
    //TODO:  Implement me
    return false;
}

bool Chip8Processor::SetRandom(uint8_t xRegister, uint8_t mask)
{
    //TODO:  Implement me
    return false;
}

bool Chip8Processor::DrawSprite(uint8_t xRegister, uint8_t yRegister, uint8_t sizeInBytes)
{
    //TODO:  Implement me
    return false;
}

bool Chip8Processor::SkipKeyPress(uint8_t xRegister, bool ifIsPressed)
{
    //TODO:  Implement me
    return false;
}

bool Chip8Processor::StoreDelayTimer(uint8_t xRegister)
{
    //TODO:  Implement me
    return false;
}

bool Chip8Processor::WaitAndStoreKey(uint8_t xRegister)
{
    //TODO:  Implement me
    return false;
}

bool Chip8Processor::SetDelayTimer(uint8_t xRegister)
{
    //TODO:  Implement me
    return false;
}

bool Chip8Processor::SetSoundTimer(uint8_t xRegister)
{
    //TODO:  Implement me
    return false;
}

bool Chip8Processor::AddToI(uint8_t xRegister)
{
    //TODO:  Implement me
    return false;
}

bool Chip8Processor::SetIToChar(uint8_t xRegister)
{
    //TODO:  Implement me
    return false;
}

bool Chip8Processor::StoreBCD(uint8_t xRegister)
{
    //TODO:  Implement me
    return false;
}

bool Chip8Processor::StoreRegs(uint8_t xRegister)
{
    //TODO:  Implement me
    return false;
}

bool Chip8Processor::FillRegs(uint8_t xRegister)
{
    //TODO:  Implement me
    return false;
}

}
