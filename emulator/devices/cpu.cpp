#include "cpu.hpp"
#include "../../common/instruction.hpp"

CPU::CPU(Memory* memory) : halt(false), term_out(0xffffff00), term_in(0xffffff04), memory(memory) {
    reset();
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
    memory->write(term_out, -1);
}

void CPU::executeInstruction() {
    
    uint32 instruction = static_cast<uint32>(memory->read(registers[PC]));
    registers[PC] += 4;

    OperationCode op = static_cast<OperationCode>(instruction >> 28);
    int mod = (instruction >> 24) & 0xf;
    int ra = (instruction >> 20) & 0xf;
    int rb = (instruction >> 16) & 0xf;
    int rc = (instruction >> 12) & 0xf;
    int disp = instruction & 0xfff;

    if (disp & 0x800) { // sign extend the 12-bit displacement
        disp |= 0xfffff000;
    }

    Instruction* instr = Instruction::binaryInstructions[op](mod, ra, rb, rc, disp);

    instr -> execute(this);

    delete instr;
}

void CPU::printContext() {
    std::cout << "\n---------------------------------------------------------------\n";
    std::cout << "Emulated processor executed halt instruction\n";
    std::cout << "Emulated processor state:\n";

    auto printReg = [&](int idx) {
        std::string label = "r" + std::to_string(idx);
        int pad = 3 - static_cast<int>(label.size());
        if (pad < 0) pad = 0;
        std::cout << label << std::string(pad, ' ') << "= "
                  << "0x" << std::hex << std::setw(8) << std::setfill('0')
                  << static_cast<uint32>(registers[idx])
                  << std::dec << std::setfill(' ');
    };

    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            int idx = row * 4 + col;
            printReg(idx);
            if (col != 3) std::cout << ' ';
        }
        std::cout << "\n";
    }
}