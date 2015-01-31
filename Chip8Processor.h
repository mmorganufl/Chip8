#ifndef CHIP8PROCESSOR_H_
#define CHIP8PROCESSOR_H_

#include <stdint.h>
#include <thread>
#include <mutex>

namespace chip8
{
class Chip8Processor
{
    static const uint16_t RAM_SIZE = 0x1000;  // 4k
    static const uint8_t  STACK_SIZE = 16;
    static const uint16_t ROM_OFFSET = 0x200;

    enum MathCode
    {
        MATH_SET = 0,
        MATH_OR = 1,
        MATH_AND = 2,
        MATH_XOR = 3,
        MATH_ADD = 4,
        MATH_SUB = 5,
        MATH_SR = 6,
        MATH_MINUS = 7,
        MATH_SL = 14
    };
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
    // Registers
    uint8_t  _v[16];

    // Program counter
    uint16_t _pc;

    // Stack pointer
    uint16_t _sp;

    // I register
    uint16_t _I;

    // Timers
    uint16_t _delayTimer;
    uint16_t _soundTimer;

    uint8_t  _RAM[RAM_SIZE];
    uint16_t _stack[STACK_SIZE];

    // True when execution thread is running
    bool            _run;
    std::mutex      _runLock;
    std::thread*    _runThread;
    std::thread*    _timerThread;

    bool HandleInstruction(uint16_t instruction);
    void ExecutionThread();
    void TimerThread();

    // Instructions
    bool ClearScreen();
    bool Return();
    bool Jump(uint16_t address);
    bool Call(uint16_t address);
    bool SkipValue(uint8_t xRegister, uint8_t value, bool ifEqual);
    bool SkipXY(uint8_t xRegister, uint8_t yRegister, bool ifEqual);
    bool SetByValue(uint8_t xRegister, uint8_t value);
    bool AddToRegister(uint8_t xRegister, uint8_t value);
    bool Math(uint8_t xRegister, uint8_t yRegister, MathCode code);
    bool SetIRegister(uint16_t value);
    bool SetRandom(uint8_t xRegister, uint8_t mask);
    bool DrawSprite(uint8_t xRegister, uint8_t yRegister, uint8_t sizeInBytes);
    bool SkipKeyPress(uint8_t xRegister, bool ifIsPressed);
    bool StoreDelayTimer(uint8_t xRegister);
    bool WaitAndStoreKey(uint8_t xRegister);
    bool SetDelayTimer(uint8_t xRegister);
    bool SetSoundTimer(uint8_t xRegister);
    bool AddToI(uint8_t xRegister);
    bool SetIToChar(uint8_t xRegister);
    bool StoreBCD(uint8_t xRegister);
    bool StoreRegs(uint8_t xRegister);
    bool FillRegs(uint8_t xRegister);
};
}
#endif /* CHIP8PROCESSOR_H_ */
