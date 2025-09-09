#include "cpu.hpp"
#include "../../common/instruction.hpp"

CPU::CPU(Memory* memory) : halt(false), memory(memory) {
    reset();
}

CPU::~CPU() {
    delete memory;
}

void CPU::reset() {
    for (int i = 0; i < 15; i++) {
        registers[i] = 0;
    }
    registers[PC] = 0x40000000;
    registers[SP] = 0xffffff00; // stack pointer initialized to the end of memory
    csr[CSRREG::STATUS] = 0;
    csr[CSRREG::HANDLER] = 0;
    csr[CSRREG::CAUSE] = 0;
}

void CPU::executeInstruction() {
    
    uint32 instruction = static_cast<uint32>(memory->read(registers[PC]));
    registers[PC] += 4; // Increment PC

    OperationCode op = static_cast<OperationCode>(instruction >> 28);
    int mod = (instruction >> 24) & 0xf;
    int ra = (instruction >> 20) & 0xf;
    int rb = (instruction >> 16) & 0xf;
    int rc = (instruction >> 12) & 0xf;
    int disp = instruction & 0xfff;

    Instruction* instr = Instruction::binaryInstructions[op](mod, ra, rb, rc, disp);

    std::cout << "PC: " << std::hex << getRegister(PC)-4 << ": Executing " << mnemonics[op] << " instruction\n";

    instr -> execute(this);

    delete instr;
}

void CPU::handleInterrupt() {
    // TODO: Implementation of interrupt handling 
}