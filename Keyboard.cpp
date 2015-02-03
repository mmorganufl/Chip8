#include "Keyboard.h"
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

namespace chip8
{

Keyboard::Keyboard()
{}

Keyboard::~Keyboard()
{}

bool Keyboard::IsKeyDown(uint8_t key)
{
    // TODO:  implement me
    return false;
}

uint8_t Keyboard::WaitForKey()
{
    // TODO:  implement me
    return 0;
}

} /* namespace chip8 */
