#ifndef CHIP8PROCESSOR_H_
#define CHIP8PROCESSOR_H_

#include <stdint.h>
#include <thread>
#include <mutex>

namespace chip8
{
class Chip8Processor
{
    typedef uint8_t Chip8DataRegister;
    static const uint16_t RAM_SIZE = 0x1000;  // 4k
    static const uint16_t ROM_OFFSET = 0x200;

public:
    /**
     * Constructor
     */
    Chip8Processor();

    /**
     * Destructor
     */
    virtual ~Chip8Processor();

    /**
     * Loads a ROM from a buffer and places it into RAM
     * @param src The address of the ROM
     * @length The length of the ROM in bytes
     * @return Returns true if the ROM was successfully loaded into memory
     */
    bool LoadRom(const uint8_t* src, uint16_t length);

    /**
     * Resets the internal state of the processor.  All registers are zeroed out,
     * except the PC which is set to ROM_OFFSET.  All timers are reset and paused.
     * @return True if the processor is reset
     */
    bool Reset();

    /**
     * Begins execution at the current PC.  Reset should be called first
     * to initialize all values.
     * @return Returns true if the processor is started
     */
    bool Run();

    /**
     * Stops program execution
     * @return Returns true if the processor is stopped
     */
    bool Stop();

    /**
     * Returns true if the processor is currently executing a program
     * @return True if the processor is currently executing a program
     */
    bool IsRunning();

protected:
    Chip8DataRegister _v0;
    Chip8DataRegister _v1;
    Chip8DataRegister _v2;
    Chip8DataRegister _v3;
    Chip8DataRegister _v4;
    Chip8DataRegister _v5;
    Chip8DataRegister _v6;
    Chip8DataRegister _v7;
    Chip8DataRegister _v8;
    Chip8DataRegister _v9;
    Chip8DataRegister _vA;
    Chip8DataRegister _vB;
    Chip8DataRegister _vC;
    Chip8DataRegister _vD;
    Chip8DataRegister _vE;
    Chip8DataRegister _vF;
    uint16_t _pc;
    uint16_t _sp;
    uint16_t _I;
    uint16_t _delayTimer;
    uint16_t _soundTimer;
    uint8_t _RAM[RAM_SIZE];

    bool            _run;
    std::mutex      _runLock;
    std::thread*    _runThread;
    std::thread*    _timerThread;

    bool HandleInstruction(uint16_t instruction);
    void ExecutionThread();
    void TimerThread();
};
}
#endif /* CHIP8PROCESSOR_H_ */
