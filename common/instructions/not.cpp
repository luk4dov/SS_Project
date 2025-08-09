#include "not.hpp"

Instruction* NotInstruction::createInstruction(short reg1, short reg2, uint32 immediate, std::string label, Section* section, std::unordered_map<std::string, Symbol*>& symbolTable, short type) {
    return new NotInstruction(reg1, section);
}

int NotInstruction::write_section_data() {
    uint32 binary = serialize(LOGIC, NOT, reg, reg, 0, 0x0); // reg <= ~reg
    write_binary(section, binary);
    return 4;
}