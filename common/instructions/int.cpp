#include "int.hpp"

Instruction* IntInstruction::parsedInstruction(const std::string& instr, int reg1, int reg2, uint32 immediate, const std::string& label, int type) {
    return new IntInstruction();
}

Instruction* IntInstruction::binaryInstruction(int mod, int reg1, int reg2, int reg3, int disp) {
    return new IntInstruction();
}

int IntInstruction::writeSectionData(Section* section, std::unordered_map<std::string, Symbol*>& symbolTable) {
    section->data.push_back(0x00);
    section->data.push_back(0x00);
    section->data.push_back(0x00);
    section->data.push_back(0x10);
    return 4;
}

void IntInstruction::execute(CPU* cpu) {
    // push status; push pc; cause <= 4; status <= status & ~(0x1); pc <= handle

    uint32 sp = cpu->getRegister(SP);
    uint32 status = cpu->getCSR(STATUS);
    uint32 pc = cpu->getRegister(PC);

    cpu->writeMem(sp-4, status);
    cpu->writeMem(sp-8, pc);

    std::cout << "OK\n";

    cpu->setRegister(SP, sp - 8);
    cpu->setCSR(CAUSE, 4);
    cpu->setCSR(STATUS, status & ~(0x1));
    uint32 handle = cpu->getCSR(CSRREG::HANDLER);
    
    std::cout << "Address of interrupt routine is " << handle << '\n';

    cpu->setRegister(PC, handle);


}