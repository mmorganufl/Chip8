#include "Beeper.h"

#define LOG_TAG "Beeper"
#include "log.h"

namespace chip8
{

Beeper::Beeper()
: _isBeeping(false)
{
}

Beeper::~Beeper()
{
}

bool Beeper::StartBeeping()
{
    _beepLock.lock();
    if (!_isBeeping)
    {
        // TODO:  implement me
    }
    _beepLock.unlock();
    return true;
}

bool Beeper::StopBeeping()
{
    _beepLock.lock();
    if (_isBeeping)
    {
        // TODO:  implement me
    }
    _beepLock.unlock();
    return true;
}

} /* namespace chip8 */
