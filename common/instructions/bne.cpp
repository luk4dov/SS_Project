#include "bne.hpp"
#include "jmp.hpp"

BneInstruction::BneInstruction(short r1, short r2, uint32 immediate, std::string label, Section* section,  std::unordered_map<std::string, Symbol*>& symbolTable, short type, JMPCondition cond) :
                                r1(r1), r2(r2), immediate(immediate), label(label), section(section), symbolTable(symbolTable), type(type), cond(cond)  {}

Instruction* BneInstruction::createInstruction(short reg1, short reg2, uint32 immediate, std::string label, Section* section, std::unordered_map<std::string, Symbol*>& symbolTable, short type) {
    return new BneInstruction(reg1, reg2, immediate, label, section, symbolTable, type, BNE);
}

int BneInstruction::write_section_data() {
    JmpInstruction* bne = new JmpInstruction(r1, r2, immediate, label, section, symbolTable, type, cond);
    int ret = bne->write_section_data();
    delete bne;
    return ret;
}