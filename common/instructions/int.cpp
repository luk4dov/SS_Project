#include "int.hpp"

Instruction* IntInstruction::parsedInstruction(const std::string& instr, int reg1, int reg2, uint32 immediate, const std::string& label, int type) {
    return new IntInstruction();
}

Instruction* IntInstruction::binaryInstruction(int mod, int reg1, int reg2, int reg3, int disp) {
    return new IntInstruction();
}

int IntInstruction::writeSectionData(Section* section, std::unordered_map<std::string, Symbol*>& symbolTable) {
    section->data.push_back(0x01);
    section->data.push_back(0x00);
    section->data.push_back(0x00);
    section->data.push_back(0x00);
    return 4;
}