#include "sub.hpp"

Instruction* SubInstruction::createInstruction(short reg1, short reg2, uint32 immediate, std::string label, Section* section, std::unordered_map<std::string, Symbol*>& symbolTable, short) {
    return new SubInstruction(reg1, reg2, section);
}

int SubInstruction::write_section_data() {
    uint32 binary = serialize(ARITH, SUB, rd, rd, rs, 0x0); // reg2 <= reg2 - reg1 -> ra <= rb - rc
    write_binary(section, binary);
    return 4;
}