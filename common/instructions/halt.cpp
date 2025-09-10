#include "halt.hpp"

Instruction* HaltInstruction::parsedInstruction(const std::string& instr, int r1, int r2, uint32 immediate, const std::string& label, int type) {
    return new HaltInstruction();
}

Instruction* HaltInstruction::binaryInstruction(int mod, int reg1, int reg2, int reg3, int disp) {
    return new HaltInstruction();
}

int HaltInstruction::writeSectionData(Section* section, std::unordered_map<std::string, Symbol*>& symbolTable) {
    write_binary(section, 0x0);
    return 4;
}

void HaltInstruction::execute(CPU* cpu) {
    cpu->setHalt();
}