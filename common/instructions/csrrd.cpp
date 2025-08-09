#include "csrrd.hpp"

Instruction* CsrrdInstruction::createInstruction(short reg1, short reg2, uint32 immediate, std::string label, Section* section, std::unordered_map<std::string, Symbol*>& symbolTable, short mn) {
    return new CsrrdInstruction(reg1, label, section);
}

int CsrrdInstruction::write_section_data() {
    uint32 binary;
    if(csr == "status") {
        binary = serialize(LD, 0, rd, STATUS, 0, 0);
    } else if(csr == "handler") {
        binary = serialize(LD, 0, rd, HANDLER, 0, 0);
    } else if (csr == "cause") {
        binary = serialize(LD, 0, rd, CAUSE, 0, 0);
    } else {
        // error
        return -1;
    }
    write_binary(section, binary);
    return 4;
}