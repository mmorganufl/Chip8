
#ifndef CHIP8PROCESSOR_H_
#define CHIP8PROCESSOR_H_

#include <stdint.h>



class Chip8Processor
{
    typedef uint16_t Chip8Register;
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
    bool LoadRom(unsigned char* src, uint16_t length);

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
     * Resets the internal state of the processor.  All registers are zeroed out,
     * except the PC which is set to ROM_OFFSET.  All timers are reset and paused.
     * @return True if the processor is reset
     */
    bool Reset();

    /**
     * Returns true if the processor is currently executing a program
     * @return True if the processor is currently executing a program
     */
    bool IsRunning();

protected:
    Chip8Register _v0;
    Chip8Register _v1;
    Chip8Register _v2;
    Chip8Register _v3;
    Chip8Register _v4;
    Chip8Register _v5;
    Chip8Register _v6;
    Chip8Register _v7;
    Chip8Register _v8;
    Chip8Register _v9;
    Chip8Register _vA;
    Chip8Register _vB;
    Chip8Register _vC;
    Chip8Register _vD;
    Chip8Register _vE;
    Chip8Register _vF;
    Chip8Register _pc;
    Chip8Register _I;

    bool _isRunning;
    bool Step();
    bool HandleOpCode(uint16_t opcode);
};

#endif /* CHIP8PROCESSOR_H_ */
