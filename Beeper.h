#ifndef BEEPER_H_
#define BEEPER_H_

#include <mutex>

namespace chip8
{

    class Beeper
    {
    public:
        Beeper();
        virtual ~Beeper();

        bool StartBeeping();
        bool StopBeeping();

    protected:
        std::mutex _beepLock;
        bool _isBeeping;
    };

} /* namespace chip8 */

#endif /* BEEPER_H_ */
