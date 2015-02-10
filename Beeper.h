#ifndef BEEPER_H_
#define BEEPER_H_

#include <mutex>
#include <thread>

namespace chip8
{

    class Beeper
    {
    public:
        Beeper();
        virtual ~Beeper();

        bool StartBeeping();
        bool StopBeeping();
        void BeepThread();

    protected:
        std::mutex      _beepLock;
        bool            _isBeeping;
        std::thread*    _beepThread;
        bool            _isAlive;
    };

} /* namespace chip8 */

#endif /* BEEPER_H_ */
