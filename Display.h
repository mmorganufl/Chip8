#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <stdint.h>
#include <bitset>
#include <thread>
#include <ncurses.h>

namespace chip8
{
    class Display
    {
        static const uint8_t  DISP_WIDTH    = 64;
        static const uint8_t  DISP_HEIGHT   = 32;

    public:
        Display();
        virtual ~Display();

        /**
         * Clears the display
         */
        void Clear();

        /**
         * Flips the value of the pixel at (x,y)  If the pixel was
         * already set, true is returned.  Otherwise, false
         * @param x The x coordinate of the pixel to flip
         * @param y The y coordinate of the pixel to flip
         * @return True if a set pixel was unset
         */
        bool FlipPixel(uint8_t x, uint8_t y);

    protected:
        void DrawBorder();
        void RefreshThread();
        std::bitset<DISP_WIDTH> _pixels[DISP_HEIGHT];
        WINDOW*                 _win;
        bool                    _refreshRun;
        std::thread*            _refreshThread;

    };

} /* namespace chip8 */

#endif /* DISPLAY_H_ */
