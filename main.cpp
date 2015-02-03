#include "Chip8Processor.h"
#include "Keyboard.h"
#include "Display.h"
#include "Beeper.h"
#include <iostream>
int main()
{
    chip8::Display* disp = new chip8::Display();
    chip8::Chip8Processor proc(NULL, NULL, NULL);

    int x = 1;
    int y = 1;
    char c = 0;
    while (c != 'q')
    {
        c = getch();
        switch (c)
        {
            case 'w':
                y--;
                break;

            case 'a':
                x--;
                break;

            case 's':
                y++;
                break;

            case 'd':
                x++;
                break;
        }
        disp->FlipPixel(x, y);
    }
    delete disp;



}
