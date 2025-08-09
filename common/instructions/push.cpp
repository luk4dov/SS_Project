#include "push.hpp"

PushInstruction::PushInstruction(short rs, Section* section) : Instruction(), rs(rs), section(section) {}

Instruction* PushInstruction::createInstruction(short reg1, short reg2, uint32 immediate, std::string label, Section* section, std::unordered_map<std::string, Symbol*>& symbolTable, short type) {
    return new PushInstruction(reg1, section);
}

int PushInstruction::write_section_data() {
    uint32 binary = serialize(ST, 0x1, 14, 0, rs, -0x4);
    write_binary(section, binary);
    return 4;
}