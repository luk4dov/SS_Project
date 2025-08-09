#include "pop.hpp"

PopInstruction::PopInstruction(short rd, Section* section, short type) : Instruction(), rd(rd), section(section), type(type) {}

Instruction* PopInstruction::createInstruction(short reg1, short reg2, uint32 immediate, std::string label, Section* section, std::unordered_map<std::string, Symbol*>& symbolTable, short type) {
    return new PopInstruction(reg1, section, type);
}

int PopInstruction::write_section_data() {
    uint32 binary = 0;
    if(type == 0) { // regular: pop rd
        binary = serialize(LD, 3, rd, 14, 0, 4);
    } else { // pop csr
        binary = serialize(LD, 0x7, rd, 14, 0, 4); 
    }
    write_binary(section, binary);
    return 4;
}