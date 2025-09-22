#include "shift.hpp"

Instruction* ShiftInstruction::parsedInstruction(const std::string& instr, int r1, int r2, uint32 imm, const std::string& label, int type) {
    ShiftOC mod = LEFT;

    if(instr == "shr") {
        mod = RIGHT;
    }

    return new ShiftInstruction(r2, r2, r1, mod);
}

Instruction* ShiftInstruction::binaryInstruction(int r1, int r2, int r3, int mod, int disp) {
    return new ShiftInstruction(r1, r2, r3, ShiftOC(mod));
}

int ShiftInstruction::writeSectionData(Section* section, std::unordered_map<std::string, Symbol*>& symbolTable) {
    uint32 binary = serialize(SHIFT, mod, r1, r2, r3, 0x0);
    write_binary(section, binary);
    return 4;
}

void ShiftInstruction::execute(CPU* cpu) {
    switch(mod) {
        case LEFT: {
            cpu->setRegister((REGS)r1, cpu->getRegister((REGS)r2) << cpu->getRegister((REGS)r3));
            break;
        }
        case RIGHT: {
            cpu->setRegister((REGS)r1, cpu->getRegister((REGS)r2) >> cpu->getRegister((REGS)r3));
        }
    }
}
