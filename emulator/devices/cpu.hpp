#ifndef CPU_HPP
#define CPU_HPP

#include "../../common/types.hpp"
#include "memory.hpp"

static std::vector<std::string> mnemonics = {"HALT", "INT", "CALL", "JMP", "XCHG", "ARITH", "LOGIC", "SHIFT", "STORE", "LOAD" }; 
enum {R0 = 0, R1, R2, R3, R4, R5, R6, R7, R8, R9, R10, R11, R12, R13, SP, PC} typedef REGS;

class CPU {
public:

    CPU(Memory*);

    void reset();
    void executeInstruction();

    const bool getHalt() const { return halt; }
    void setHalt() { halt = true; }

    void setRegister(REGS r, int val) { 
        if(r == REGS::R0) return;
        registers[r] = val; 
    }
    int getRegister(REGS r) { return registers[r]; }

    void setCSR(CSRREG r, int val) { csr[r] = val; }
    uint32 getCSR(CSRREG r) { return csr[r]; }

    void writeMem(uint32 address, int data) { memory->write(address, data); }
    uint32 readMem(uint32 address) { return memory->read(address); }

    const uint32 getTermOut() const { return this->term_out; }
    const uint32 getTermIn() const { return this->term_in; }

    void printContext();

private:
    bool halt;

    int registers[16];
    uint32 csr[3]; // status, handler, cause
    const uint32 term_out;
    const uint32 term_in;

    Memory* memory;

};

#endif