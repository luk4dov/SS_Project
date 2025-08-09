#ifndef INSTRUCTION_DIV_HPP
#define INSTRUCTION_DIV_HPP

#include "../instruction.hpp"

class DivInstruction : public Instruction {
public:
    DivInstruction(short rs, short rd, Section* section) : Instruction(), rs(rs), rd(rd), section(section) {}
    ~DivInstruction() override {};

    static Instruction* createInstruction(short, short, uint32, std::string, Section*, std::unordered_map<std::string, Symbol*>&, short);

    int write_section_data() override;

private:
    short rs;
    short rd;
    Section* section;
};

#endif