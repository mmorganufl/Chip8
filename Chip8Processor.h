
#ifndef CHIP8PROCESSOR_H_
#define CHIP8PROCESSOR_H_

#include <stdint.h>



class Chip8Processor
{
    typedef uint16_t Chip8Register;
    static const uint16_t RAM_SIZE = 4096;

public:
    Chip8Processor();
    virtual ~Chip8Processor();

    bool LoadRom(unsigned char* src, uint16_t length);
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
};

#endif /* CHIP8PROCESSOR_H_ */
