#include "xchg.hpp"

Instruction* XchgInstruction::createInstruction(short reg1, short reg2, uint32 immediate, std::string label, Section* section, std::unordered_map<std::string, Symbol*>& symbolTable, short type) {
    return new XchgInstruction(reg1, reg2, section);
} 

int XchgInstruction::write_section_data() {
    uint32 binary = serialize(ASWAP, 0, 0, rs, rd, 0);
    write_binary(section, binary);
    return 4;
}