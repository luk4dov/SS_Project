#ifndef CPU_HPP
#define CPU_HPP

#include "../../common/types.hpp"
#include "./memory.hpp"

enum {R0 = 0, R1, R2, R3, R4, R5, R6, R7, R8, R9, R10, R11, R12, R13, SP, PC} typedef REGS;
enum { INVALID_INSTRUCTION = 1, TIMER, TERMINAL, SOFT_INT } typedef INT_CAUSE;
enum { TIMER_STATUS, TERMINAL_STATUS, GLOBAL } typedef INT_MASK;

class CPU {
public:

    CPU(Memory*);

    void reset();
    void executeInstruction();
    void interrupt(INT_CAUSE);

    bool getHalt() { return halt; }
    void setHalt() { halt = true; }

    void setRegister(REGS r, int val) { if(r == REGS::R0) return; registers[r] = val; }
    int getRegister(REGS r) { return registers[r]; }

    void setCSR(CSRREG r, int val) { csr[r] = val; }
    uint32 getCSR(CSRREG r) { return csr[r]; }

    void writeMem(uint32 address, int data) { memory->write(address, data); }
    uint32 readMem(uint32 address) { return memory->read(address); }

    uint32 getTermOut() { return this->term_out; }
    uint32 getTermIn() { return this->term_in; }
    uint32 getTimCfg() { return this->tim_cfg; }

    void printContext();

private:
    bool halt;

    int registers[16];
    uint32 csr[3]; // status, handler, cause
    const uint32 term_out;
    const uint32 term_in;

    const uint32 tim_cfg;

    Memory* memory;

};

#endif
