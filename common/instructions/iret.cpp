#include "iret.hpp"
#include "load.hpp"

Instruction* IretInstruction::createInstruction(short reg1, short reg2, uint32 immediate, std::string label, Section* section, std::unordered_map<std::string, Symbol*>& symbolTable, short type) {
    return new IretInstruction(section);
}

int IretInstruction::write_section_data() {
    uint32 binary = serialize(LD, 0x6, STATUS, 14, 0, 4);
    write_binary(section, binary);
    binary = serialize(LD, 0x3, 15, 14, 0, 8);
    write_binary(section, binary);
    return 8;
}