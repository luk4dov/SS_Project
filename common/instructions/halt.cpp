#include "halt.hpp"

Instruction* HaltInstruction::createInstruction(short reg1, short reg2, uint32 immediate, std::string label, Section* section, std::unordered_map<std::string, Symbol*>& symbolTable, short type) {
    return new HaltInstruction(section);
}

int HaltInstruction::write_section_data() {
    section->data.push_back(0x00);
    section->data.push_back(0x00);
    section->data.push_back(0x00);
    section->data.push_back(0x00);
    return 4;
}