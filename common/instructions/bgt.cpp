#include "bgt.hpp"
#include "jmp.hpp"

BgtInstruction::BgtInstruction(short r1, short r2, uint32 immediate, std::string label, Section* section,  std::unordered_map<std::string, Symbol*> symbolTable, short type, JMPCondition cond) :
                                r1(r1), r2(r2), immediate(immediate), label(label), section(section), symbolTable(symbolTable), type(type), cond(cond)  {}

Instruction* BgtInstruction::createInstruction(short reg1, short reg2, uint32 immediate, std::string label, Section* section, std::unordered_map<std::string, Symbol*>& symbolTable, short type) {
    return new BgtInstruction(reg1, reg2, immediate, label, section, symbolTable, type, BGT);
}

int BgtInstruction::write_section_data() {
    JmpInstruction* bgt = new JmpInstruction(r1, r2, immediate, label, section, symbolTable, type, cond);
    int ret = bgt->write_section_data();
    delete bgt;
    return ret;
}