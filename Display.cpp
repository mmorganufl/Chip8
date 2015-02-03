#include "Display.h"

namespace chip8
{

Display::Display()
{
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    _win = newwin(DISP_HEIGHT+2, DISP_WIDTH+2, 0, 0);
    DrawBorder();
}

Display::~Display()
{
    endwin();
}

void Display::DrawBorder()
{
    wborder(_win, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
    touchwin(_win);
    refresh();
    wrefresh(_win);
}

void Display::Clear()
{
    wclear(_win);
    DrawBorder();
}

bool Display::FlipPixel(uint8_t x, uint8_t y)
{
    bool isSet = _pixels[x][y];
    _pixels[x][y] = isSet ^ true;
    if (isSet)
    {
        mvwaddch(_win, y+1, x+1, ' ');
    }
    else
    {
        mvwaddch(_win, y+1, x+1, ACS_BLOCK);
    }
    refresh();
    wrefresh(_win);
    return isSet;
}
} /* namespace chip8 */
