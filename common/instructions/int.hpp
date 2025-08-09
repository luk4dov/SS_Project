#ifndef INSTRUCTION_INT_HPP
#define INSTRUCTION_INT_HPP

#include "../instruction.hpp"

class IntInstruction : public Instruction {
public:
    IntInstruction(Section* section) : Instruction(), section(section) {}
    ~IntInstruction() override {};

    static Instruction* createInstruction(short, short, uint32, std::string, Section*, std::unordered_map<std::string, Symbol*>&, short);

    int write_section_data() override;
private:
    Section* section;
};


#endif