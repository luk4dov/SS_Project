#include "csrwr.hpp"

Instruction* CsrwrInstruction::createInstruction(short reg1, short reg2, uint32 immediate, std::string label, Section* section, std::unordered_map<std::string, Symbol*>& symbolTable, short mn) {
    return new CsrwrInstruction(reg1, label, section);
}

int CsrwrInstruction::write_section_data() {
    uint32 binary;
    if(csr == "status") {
        binary = serialize(LD, 0x4, STATUS, rs, 0, 0);
    } else if(csr == "handler") {
        binary = serialize(LD, 0x4, HANDLER, rs, 0, 0);
    } else if (csr == "cause") {
        binary = serialize(LD, 0x4, CAUSE, rs, 0, 0);
    } else {
        // error
        return -1;
    }
    write_binary(section, binary);
    return 4;
}