#include "beq.hpp"
#include "jmp.hpp"

BeqInstruction::BeqInstruction(short r1, short r2, uint32 immediate, std::string label, Section* section,  std::unordered_map<std::string, Symbol*>& symbolTable, short type, JMPCondition cond) :
                                r1(r1), r2(r2), immediate(immediate), label(label), section(section), symbolTable(symbolTable), type(type), cond(cond)  {}

Instruction* BeqInstruction::createInstruction(short reg1, short reg2, uint32 immediate, std::string label, Section* section, std::unordered_map<std::string, Symbol*>& symbolTable, short type) {
    return new BeqInstruction(reg1, reg2, immediate, label, section, symbolTable, type, BEQ);
}

int BeqInstruction::write_section_data() {
    JmpInstruction* beq = new JmpInstruction(r1, r2, immediate, label, section, symbolTable, type, cond);
    int ret = beq->write_section_data();
    delete beq;
    return ret;
}