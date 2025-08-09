#ifndef INSTRUCTION_NOT_HPP
#define INSTRUCTION_NOT_HPP

#include "../instruction.hpp"

class NotInstruction : public Instruction {
public:
    NotInstruction(short reg, Section* section) : Instruction(), reg(reg), section(section) {}
    ~NotInstruction() override {};

    static Instruction* createInstruction(short, short, uint32, std::string, Section*, std::unordered_map<std::string, Symbol*>&, short);

    int write_section_data() override;

private:
    short reg;
    Section* section;
};

#endif