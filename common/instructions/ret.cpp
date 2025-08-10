#include "ret.hpp"
#include "pop.hpp"

Instruction* RetInstruction::createInstruction(short reg1, short reg2, uint32 immediate, std::string label, Section* section, std::unordered_map<std::string, Symbol*>& symbolTable, short mn) {
    return new RetInstruction(section);
}

int RetInstruction::write_section_data() {
    PopInstruction* pop = new PopInstruction(15, section, 0);
    int ret = pop -> write_section_data();
    delete pop;
    return ret;
}