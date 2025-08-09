#ifndef INSTRUCTION_MUL_HPP
#define INSTRUCTION_MUL_HPP

#include "../instruction.hpp"

class MulInstruction : public Instruction {
public:
    MulInstruction(short rs, short rd, Section* section) : Instruction(), rs(rs), rd(rd), section(section) {}
    ~MulInstruction() override {};

    static Instruction* createInstruction(short, short, uint32, std::string, Section*, std::unordered_map<std::string, Symbol*>&, short);

    int write_section_data() override;

private:
    short rs;
    short rd;
    Section* section;
};

#endif