#include "Beeper.h"
#include <chrono>
#include <ncurses.h>

#define LOG_TAG "Beeper"
#include "log.h"

namespace chip8
{

Beeper::Beeper()
: _isBeeping(false)
, _isAlive(true)
{
    _beepThread = new std::thread(&Beeper::BeepThread, this);
}

Beeper::~Beeper()
{
    _isAlive = false;
    _beepThread->join();
    delete _beepThread;
}

bool Beeper::StartBeeping()
{
    _beepLock.lock();
    if (!_isBeeping)
    {
        _isBeeping = true;
    }
    _beepLock.unlock();
    return true;
}

bool Beeper::StopBeeping()
{
    _beepLock.lock();
    if (_isBeeping)
    {
        _isBeeping = false;
    }
    _beepLock.unlock();
    return true;
}

void Beeper::BeepThread()
{
    while (_isAlive)
    {
        if (_isBeeping)
        {
            beep();
        }
        std::chrono::milliseconds period(50);
        std::this_thread::sleep_for(period);
    }
}
} /* namespace chip8 */
