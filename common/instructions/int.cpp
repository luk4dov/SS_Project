#include "int.hpp"

Instruction* IntInstruction::createInstruction(short reg1, short reg2, uint32 immediate, std::string label, Section* section, std::unordered_map<std::string, Symbol*>& symbolTable, short type) {
    return new IntInstruction(section);
}

int IntInstruction::write_section_data() {
    section->data.push_back(0x01);
    section->data.push_back(0x00);
    section->data.push_back(0x00);
    section->data.push_back(0x00);
    return 4;
}