#include "shl.hpp"

ShlInstruction::ShlInstruction(short rs, short rd, Section* section) : Instruction(), rs(rs), rd(rd), section(section) {}

Instruction* ShlInstruction::createInstruction(short reg1, short reg2, uint32 immediate, std::string label, Section* section, std::unordered_map<std::string, Symbol*>& symbolTable, short type) {
    return new ShlInstruction(reg1, reg2, section);
}

int ShlInstruction::write_section_data() {
    uint32 binary = serialize(SHIFT, LEFT, rd, rd, rs, 0x0); // reg2 <= reg2 << reg1 -> ra <= rb << rc
    write_binary(section, binary);
    return 4;
}