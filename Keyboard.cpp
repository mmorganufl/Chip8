#include "Keyboard.h"
#include <ncurses.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <linux/input.h>

#define LOG_TAG "Keyboard"
#include "log.h"
namespace chip8
{

    Keyboard::Keyboard()
    {
    }

    Keyboard::~Keyboard()
    {
    }

    bool Keyboard::IsKeyDown(uint8_t key)
    {
        static const char keyMap[] =
        {
            KEY_X, // 0
            KEY_1, // 1
            KEY_2, // 2
            KEY_3, // 3
            KEY_Q, // 4
            KEY_W, // 5
            KEY_E, // 6
            KEY_A, // 7
            KEY_S, // 8
            KEY_D, // 9
            KEY_Z, // 10
            KEY_C, // 11
            KEY_4, // 12
            KEY_R, // 13
            KEY_F, // 14
            KEY_V, // 15
        };

        if (key >= sizeof(keyMap) / sizeof(keyMap[0]))
        {
            return false;
        }

        int keyCode = keyMap[key];
        FILE *kbd = fopen("/dev/input/by-path/platform-i8042-serio-0-event-kbd", "r");

        char key_map[KEY_MAX/8 + 1];    //  Create a byte array the size of the number of keys

        memset(key_map, 0, sizeof(key_map));    //  Initate the array to zero's
        ioctl(fileno(kbd), EVIOCGKEY(sizeof(key_map)), key_map);    //  Fill the keymap with the current keyboard state

        int keyb = key_map[keyCode/8];  //  The key we want (and the seven others arround it)
        int mask = 1 << (keyCode % 8);  //  Put a one in the same column as out key state will be in;

        bool isPressed = (keyb & mask);  //  Returns true if pressed otherwise false
        fclose(kbd);
        if (isPressed)
        {
            beep();
        }

        return isPressed;
    }


    uint8_t Keyboard::WaitForKey()
    {
        char key = getch();
        switch (key)
        {
        case '1':
            return 1;
            break;

        case '2':
            return 2;
            break;

        case '3':
            return 3;
            break;

        case 'q':
            return 4;
            break;

        case 'w':
            return 5;
            break;

        case 'e':
            return 6;
            break;

        case 'a':
            return 7;
            break;

        case 's':
            return 8;
            break;

        case 'd':
            return 9;
            break;

        case 'z':
            return 10;
            break;

        case 'x':
            return 0;
            break;

        case 'c':
            return 11;
            break;

        case '4':
            return 12;
            break;

        case 'r':
            return 13;
            break;

        case 'f':
            return 14;
            break;

        case 'v':
            return 15;
            break;
        }
        return 0x10;
    }
} /* namespace chip8 */
