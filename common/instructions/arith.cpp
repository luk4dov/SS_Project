#include "arith.hpp"


Instruction* ArithInstruction::parsedInstruction(const std::string& instr, int r1, int r2, uint32 imm, const std::string& label, int type) {
    ArithOC mod = ADD;

    if(instr == "sub") {
        mod = SUB;
    } else if(instr == "mul") {
        mod = MUL;
    } else if(instr == "div") {
        mod = DIV;
    }

    return new ArithInstruction(r2, r2, r1, mod);
}

Instruction* ArithInstruction::binaryInstruction(int mod, int r1, int r2, int r3, int disp) {
    return new ArithInstruction(r1, r2, r3, ArithOC(mod));
}

int ArithInstruction::writeSectionData(Section* section, std::unordered_map<std::string, Symbol*>& symbolTable) {
    uint32 binary = serialize(ARITH, mod, r1, r2, r3, 0x0);
    write_binary(section, binary);
    return 4;
}

void ArithInstruction::execute(CPU* cpu) {
    // if destination register is 0, return 0 (r0 is always 0)
    if(r1 == 0) return;

    switch(mod) {
        case ADD: {
            cpu->setRegister((REGS)r1, cpu->getRegister((REGS)r2) + cpu->getRegister((REGS)r3));
            break;
        }
        case SUB: {
            cpu->setRegister((REGS)r1, cpu->getRegister((REGS)r2) - cpu->getRegister((REGS)r3));
            break;
        }
        case MUL: {
            cpu->setRegister((REGS)r1, cpu->getRegister((REGS)r2) * cpu->getRegister((REGS)r3));
            break;
        }
        case DIV: {
            if(cpu->getRegister((REGS)r3) == 0) {
                throw DivisionByZero();
            }
            cpu->setRegister((REGS)r1, cpu->getRegister((REGS)r2) / cpu->getRegister((REGS)r3));
        }
    }
}
