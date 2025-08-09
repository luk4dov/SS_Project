#ifndef INSTRUCTION_SUB_HPP
#define INSTRUCTION_SUB_HPP

#include "../instruction.hpp"

class SubInstruction : public Instruction {
public:
    SubInstruction(short rs, short rd, Section* section) : Instruction(), rs(rs), rd(rd), section(section) {}
    ~SubInstruction() override {};

    static Instruction* createInstruction(short, short, uint32, std::string, Section*, std::unordered_map<std::string, Symbol*>&, short);

    int write_section_data() override;

private:
    short rs;
    short rd;
    Section* section;
};

#endif