#include "log.h"
#include "Chip8Processor.h"
#include "Display.h"
#include "Keyboard.h"
#include "Beeper.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <chrono>

#define LOG_TAG "Chip8Processor"
#include "log.h"
namespace chip8
{

Chip8Processor::Chip8Processor(Keyboard* keyboard, Display* display, Beeper* beeper)
: _runThread(NULL)
, _timerThread(NULL)
, _keyboard(keyboard)
, _display(display)
, _beeper(beeper)
{
    Reset();

    uint8_t fontData[] =
    {
            0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
            0x20, 0x60, 0x20, 0x20, 0x70, // 1
            0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
            0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
            0x90, 0x90, 0xF0, 0x10, 0x10, // 4
            0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
            0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
            0xF0, 0x10, 0x20, 0x40, 0x40, // 7
            0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
            0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
            0xF0, 0x90, 0xF0, 0x90, 0x90, // A
            0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
            0xF0, 0x80, 0x80, 0x80, 0xF0, // C
            0xE0, 0x90, 0x90, 0x90, 0xE0, // D
            0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
            0xF0, 0x80, 0xF0, 0x80, 0x80 // F
    };
    memcpy(_RAM, fontData, sizeof(fontData));
}

Chip8Processor::~Chip8Processor()
{
    Stop();
}

bool Chip8Processor::LoadRom(const uint8_t* src, uint16_t length)
{
    LOG("%s: %d", __FUNCTION__, length);
    if (length > (Chip8Processor::RAM_SIZE - Chip8Processor::ROM_OFFSET))
    {
        LOG("Length is too long: %d", length);
        return false;
    }

    for (uint16_t i = 0; i < length; i++)
    {
        _RAM[i + Chip8Processor::ROM_OFFSET] = src[i];
    }
    LOG("ROM Loaded!");
    return true;
}

bool Chip8Processor::Reset()
{
    Stop();

    memset(_v, sizeof(_v), 0);
    _I = 0;
    _pc = ROM_OFFSET;
    _sp = STACK_OFFSET;
    _delayTimer = 0;
    _soundTimer = 0;

    return true;
}

bool Chip8Processor::Run()
{
    _runLock.lock();
    LOG("Got lock");
    if (!_run)
    {
        _run = true;
        _runThread = new std::thread(&Chip8Processor::ExecutionThread, this);
        LOG("Execution thread started");
        _timerThread = new std::thread(&Chip8Processor::TimerThread, this);
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

        _timerThread->join();
        delete _timerThread;
        _timerThread = NULL;
    }
    _runLock.unlock();
    return _run;
}

bool Chip8Processor::IsRunning()
{
    return _run;
}

void Chip8Processor::ExecutionThread()
{
    LOG("Starting execution thread");
    while(_run)
    {
        if (!Step())
        {
            LOG("The instruction failed to execute properly");
            return;
        }
        std::chrono::microseconds period(500);
        std::this_thread::sleep_for(period);
    }
    return;
}

bool Chip8Processor::Step()
{
    uint16_t instruction = _RAM[_pc];
    instruction <<= 8;
    instruction += _RAM[_pc+1];

    LOG("pc = 0x%x", _pc);
    return HandleInstruction(instruction);
}

void Chip8Processor::TimerThread()
{
    LOG("Starting timer thread");
    while(_run)
    {
        LOG("tick!");
        _timerLock.lock();

        if (_delayTimer != 0)
        {
            _delayTimer--;
        }


        if (_soundTimer != 0)
        {
            _soundTimer--;
            if (_soundTimer == 0)
            {
                _beeper->StopBeeping();
            }
        }
        _timerLock.unlock();
        std::chrono::microseconds period(16666);
        std::this_thread::sleep_for(period);
    }
    return;
}

bool Chip8Processor::HandleInstruction(uint16_t instruction)
{
    LOG("%s: %x", __FUNCTION__, instruction);
    uint8_t firstNibble = ((instruction & 0xF000) >> 12);

    uint8_t xRegister = (instruction & 0x0F00) >> 8;
    uint8_t yRegister = (instruction & 0x00F0) >> 4;

    LOG("xRegister: %x, yRegister: %x", xRegister, yRegister);
    _pc += 2;
    switch (firstNibble)
    {
        case 0:
        {
            if (instruction == 0x00E0)
            {
                return ClearScreen();
            }
            else if (instruction == 0x00EE)
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
    LOG_RED("%s", __FUNCTION__);
    _display->Clear();
    return true;
}

bool Chip8Processor::Return()
{
    LOG_RED("%s", __FUNCTION__);
    _pc = *((uint16_t*)(_RAM +_sp));
    _sp += 2;
    return (_sp <= STACK_OFFSET);
}

bool Chip8Processor::Jump(uint16_t address)
{
    LOG_RED("%s: %x", __FUNCTION__, address);
    _pc = address;
    return true;
}

bool Chip8Processor::Call(uint16_t address)
{
    LOG_RED("%s: %x", __FUNCTION__, address);
    _sp -= 2;
    *((uint16_t*)(_RAM + _sp)) = _pc;
    _pc = address;
    return (_sp >= (Chip8Processor::STACK_OFFSET - (2 * STACK_DEPTH)));
}

bool Chip8Processor::SkipValue(uint8_t xRegister, uint8_t value, bool ifEqual)
{
    LOG_RED("%s: V%u, %u, %s", __FUNCTION__, xRegister, value, ifEqual ? "true" : "false");
    LOG("v%d = %d", xRegister, _v[xRegister]);
    bool isEqual = (_v[xRegister] == value);
    if (isEqual == ifEqual)
    {
        LOG("skipping");
        _pc += 2;
    }
    return true;
}

bool Chip8Processor::SkipXY(uint8_t xRegister, uint8_t yRegister, bool ifEqual)
{
    LOG_RED("%s: V%u, V%u, %s", __FUNCTION__, xRegister, yRegister, ifEqual ? "true" : "false");
    bool isEqual = (_v[xRegister] == _v[yRegister]);
    if (isEqual == ifEqual)
    {
        _pc += 2;
    }
    return true;
}

bool Chip8Processor::SetByValue(uint8_t xRegister, uint8_t value)
{
    LOG_RED("%s: V%u, %u", __FUNCTION__, xRegister, value);
    _v[xRegister] = value;
    return true;
}

bool Chip8Processor::AddToRegister(uint8_t xRegister, uint8_t value)
{
    LOG_RED("%s: V%u, %u", __FUNCTION__, xRegister, value);
    _v[xRegister] += value;
    return true;
}

bool Chip8Processor::Math(uint8_t xRegister, uint8_t yRegister, MathCode code)
{
    LOG_RED("%s: V%u, V%u, code:%u", __FUNCTION__, xRegister, yRegister, (uint8_t)code);
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
            _v[15] = (_v[xRegister] > _v[yRegister]) ? 1 : 0;
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
    LOG_RED("%s: %u", __FUNCTION__, value);
    _I = value;
    return true;
}

bool Chip8Processor::SetRandom(uint8_t xRegister, uint8_t mask)
{
    LOG_RED("%s: V%u, %x", __FUNCTION__, xRegister, mask);
    std::default_random_engine randEng(_rand());
    std::uniform_int_distribution<int> uniformDist(0, 255);
    uint8_t rnd = uniformDist(randEng);
    _v[xRegister] = rnd & mask;
    return true;
}

bool Chip8Processor::DrawSprite(uint8_t xRegister, uint8_t yRegister, uint8_t sizeInBytes)
{
    LOG_RED("%s: V%u=%d, V%u=%d, I=%u, %u", __FUNCTION__, xRegister, _v[xRegister], yRegister, _v[yRegister], _I, sizeInBytes);
    if (sizeInBytes > 15)
    {
        LOG("Size too big!");
        return false;
    }
    _v[15] = 0;  // Assume no pixels are flipped

    uint8_t x = _v[xRegister];
    uint8_t y = _v[yRegister];

    for (uint8_t i = 0; i < sizeInBytes; i++)
    {
        uint8_t mask = 0x80;
        uint8_t row = _RAM[_I + i];
        for (uint8_t xPos = 0; xPos < 8; xPos++)
        {
            bool shouldFlip = ((row & mask) != 0);
            if (shouldFlip)
            {
                if (_display->FlipPixel(x + xPos, y))
                {
                    _v[15] = 1;  // Set VF if a set pixel was unset
                }
            }
            mask >>= 1;
        }
        y++;
    }
    return true;
}

bool Chip8Processor::SkipKeyPress(uint8_t xRegister, bool ifIsPressed)
{
    LOG_RED("%s: V%u, %s", __FUNCTION__, xRegister, ifIsPressed ? "true":"false");
    bool keyIsPressed = _keyboard->IsKeyDown(_v[xRegister]);
    if (keyIsPressed == ifIsPressed)
    {
        _pc += 2;
    }
    return true;
}

bool Chip8Processor::StoreDelayTimer(uint8_t xRegister)
{
    LOG_RED("%s: V%u", __FUNCTION__, xRegister);
    _v[xRegister] = _delayTimer;
    LOG("DelayTimer = %d", _delayTimer);
    return true;
}

bool Chip8Processor::WaitAndStoreKey(uint8_t xRegister)
{
    LOG_RED("%s: V%u", __FUNCTION__, xRegister);
    do {
        _v[xRegister] = _keyboard->WaitForKey();
    } while ((_v[xRegister] != 0x10) && _run);

    return true;
}

bool Chip8Processor::SetDelayTimer(uint8_t xRegister)
{
    LOG_RED("%s: V%u", __FUNCTION__, xRegister);
    _timerLock.lock();
    _delayTimer = _v[xRegister];
    _timerLock.unlock();
    return true;
}

bool Chip8Processor::SetSoundTimer(uint8_t xRegister)
{
    LOG_RED("%s: V%u", __FUNCTION__, xRegister);
    _timerLock.lock();
    _soundTimer = _v[xRegister];
    if (_soundTimer > 0)
    {
        _beeper->StartBeeping();
    }
    _timerLock.unlock();
    return true;
}

bool Chip8Processor::AddToI(uint8_t xRegister)
{
    LOG_RED("%s: V%u", __FUNCTION__, xRegister);
    _I = _I + _v[xRegister];
    return true;
}

bool Chip8Processor::SetIToChar(uint8_t xRegister)
{
    LOG_RED("%s: V%u", __FUNCTION__, xRegister);
    _I =  5 * _v[xRegister];
    return true;
}

bool Chip8Processor::StoreBCD(uint8_t xRegister)
{
    LOG_RED("%s: V%u", __FUNCTION__, xRegister);
    uint8_t value = _v[xRegister];

    // Most significant digit
    _RAM[_I] = value / 100;
    value %= 100;

    // Second significant digit
    _RAM[_I + 1] = value / 10;
    value %= 10;

    // Least significant digit
    _RAM[_I + 2] = value;
    return true;
}

bool Chip8Processor::StoreRegs(uint8_t xRegister)
{
    LOG_RED("%s: V%u", __FUNCTION__, xRegister);
    for (uint8_t i = 0; i <= xRegister; i++)
    {
        _RAM[_I + i] = _v[i];
    }
    return true;
}

bool Chip8Processor::FillRegs(uint8_t xRegister)
{
    LOG_RED("%s: V%u", __FUNCTION__, xRegister);
    for (uint8_t i = 0; i <= xRegister; i++)
    {
        _v[i] = _RAM[_I + i];
    }
    return true;
}


}
