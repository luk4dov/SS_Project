#include "logic.hpp"

Instruction* LogicInstruction::parsedInstruction(const std::string& instr, int r1, int r2, uint32 imm, const std::string& label, int type) {
    LogicOC mod = NOT;

    if(instr == "and") {
        mod = AND;
    } else if(instr == "or") {
        mod = OR;
    } else if(instr == "xor") {
        mod = XOR;
    }

    if(mod == NOT)
        return new LogicInstruction(r1, r1, 0, mod);
    return new LogicInstruction(r2, r2, r1, mod);
}

Instruction* LogicInstruction::binaryInstruction(int mod, int r1, int r2, int r3, int disp) {
    return new LogicInstruction(r1, r2, r3, LogicOC(mod));
}

int LogicInstruction::writeSectionData(Section* section, std::unordered_map<std::string, Symbol*>& symbolTable) {
    uint32 binary = serialize(LOGIC, mod, r1, r2, r3, 0x0);
    write_binary(section, binary);
    return 4;
}

void LogicInstruction::execute(CPU* cpu) {
    switch(mod) {
        case AND: {
            cpu->setRegister((REGS)r1, cpu->getRegister((REGS)r2) & cpu->getRegister((REGS)r3));
            break;
        }
        case OR: {
            cpu->setRegister((REGS)r1, cpu->getRegister((REGS)r2) | cpu->getRegister((REGS)r3));
            break;
        }
        case XOR: {
            cpu->setRegister((REGS)r1, cpu->getRegister((REGS)r2) ^ cpu->getRegister((REGS)r3));
            break;
        }
        case NOT: {
            cpu->setRegister((REGS)r1, ~cpu->getRegister((REGS)r2));
        }
    }
}