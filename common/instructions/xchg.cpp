#include "./xchg.hpp"

Instruction* XchgInstruction::parsedInstruction(const std::string& instr, int reg1, int reg2, uint32 immediate, const std::string& label, int type) {
    return new XchgInstruction(reg1, reg2, 0, 0);
}

Instruction* XchgInstruction::binaryInstruction(int mod, int r1, int r2, int r3, int disp) {
    return new XchgInstruction(r1, r2, r3, disp);
}

int XchgInstruction::writeSectionData(Section* section, std::unordered_map<std::string, Symbol*>& symbolTable) {
    uint32 binary = serialize(ASWAP, 0, 0, r1, r2, 0);
    write_binary(section, binary);
    return 4;
}

void XchgInstruction::execute(CPU* cpu) {
    if(r2 == 0 || r3 == 0) {
        if(r2 == 0) cpu->setRegister((REGS)r3, 0);
        if(r3 == 0) cpu->setRegister((REGS)r2, 0);
        return;
    }

    int temp = cpu->getRegister((REGS)r2);
    cpu->setRegister((REGS)r2, cpu->getRegister((REGS)r3));
    cpu->setRegister((REGS)r3, temp);
}
