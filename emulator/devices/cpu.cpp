#include "cpu.hpp"
#include "../../common/instruction.hpp"

CPU::CPU() : halt(false) {
    reset();
}

void CPU::reset() {
    for (int i = 0; i < 15; i++) {
        registers[i] = 0;
    }
    registers[15] = 0x40000000; // pc
    csr[0] = 0; // status
    csr[1] = 0; // handler
    csr[2] = 0; // cause
}

void CPU::executeInstruction() {
    uint32 instruction = static_cast<uint32>(memory->read(registers[15]));
    registers[15] += 4; // Increment PC

    OperationCode op = static_cast<OperationCode>(instruction >> 28);
    int mod = (instruction >> 24) & 0xf;
    int ra = (instruction >> 20) & 0xf;
    int rb = (instruction >> 16) & 0xf;
    int rc = (instruction >> 12) & 0xf;
    int disp = instruction & 0xfff;

    Instruction* instr = Instruction::binaryInstructions[op](mod, ra, rb, rc, disp);
    instr -> execute(this);

    delete instr;
}

void CPU::handleInterrupt() {
    // TODO: Implementation of interrupt handling 
}