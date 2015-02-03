#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#include <stdint.h>
namespace chip8
{
    class Keyboard
    {
    public:
        Keyboard();
        virtual ~Keyboard();

        /**
         * Returns true if the key is currently down
         * @param key The number of the key to check
         * @return True if the key is pressed
         */
        bool IsKeyDown(uint8_t key);

        /**
         * Waits for a key to be pressed and returns the
         * number of the key that is pressed
         * @return The number of the key that was pressed
         */
        uint8_t WaitForKey();
    };

} /* namespace chip8 */

#endif /* KEYBOARD_H_ */
