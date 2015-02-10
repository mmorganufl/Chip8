#include "Display.h"
#include <string.h>
#include <chrono>

#define LOG_TAG "Display"
#include "log.h"

namespace chip8
{

Display::Display()
: _refreshRun(true)
{
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    _win = newwin(DISP_HEIGHT+2, DISP_WIDTH+2, 0, 0);
    DrawBorder();
    _refreshThread = new std::thread(&Display::RefreshThread, this);
}

Display::~Display()
{
    _refreshRun = false;
    _refreshThread->join();
    delete _refreshThread;
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
    memset(_pixels, 0, sizeof(_pixels));
}

bool Display::FlipPixel(uint8_t x, uint8_t y)
{
    LOG("%s", __FUNCTION__);
    x %= DISP_WIDTH;
    y %= DISP_HEIGHT;
    bool isSet = _pixels[y][x];
    _pixels[y][x] = isSet ^ true;
    if (isSet)
    {
        mvwaddch(_win, y+1, x+1, ' ');
    }
    else
    {
        mvwaddch(_win, y+1, x+1, '\xFE');
    }
    return isSet;
}

void Display::RefreshThread()
{
    while (_refreshRun)
    {
        std::chrono::milliseconds period(40);
        std::this_thread::sleep_for(period);
        wrefresh(_win);
        refresh();
    }
}
} /* namespace chip8 */
