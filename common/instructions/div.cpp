#include "div.hpp"

Instruction* DivInstruction::createInstruction(short rs, short rd, uint32 immediate, std::string label, Section* section, std::unordered_map<std::string, Symbol*>& symbolTable, short type) {
    return new DivInstruction(rs, rd, section);
}

int DivInstruction::write_section_data() {
    uint32 binary = serialize(ARITH, DIV, rd, rd, rs, 0x0); // rd <= rd / rs -> ra <= rb / rc
    write_binary(section, binary);
    return 4;
}