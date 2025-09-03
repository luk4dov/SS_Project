#ifndef CPU_HPP
#define CPU_HPP

#include "../../common/types.hpp"
#include "memory.hpp"

enum {R0 = 0, R1, R2, R3, R4, R5, R6, R7, R8, R9, R10, R11, R12, R13, SP, PC} typedef REGS;
// enum {CAUSE = 0, HANDLE, STATUS} typedef CSR;

class CPU {
public:

    CPU();
    void reset();
    void executeInstruction();
    void handleInterrupt();

    const bool getHalt() const { return halt; }
    void setHalt() { halt = true; }

    void setRegister(REGS r, int val) { registers[r] = val; }
    int getRegister(REGS r) { return registers[r]; }

    void setCSR(CSRREG r, int val) { csr[r] = val; }
    uint32 getCSR(CSRREG r) { return csr[r]; }

    void writeMem(uint32 address, int data) { memory->write(address, data); }
    uint32 readMem(uint32 address) { return memory->read(address); }

private:
    bool halt;

    int registers[16];
    uint32 csr[3]; // status, handler, cause
    
    Memory* memory;
};

#endif