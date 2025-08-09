#ifndef INSTRUCTION_ADD_HPP
#define INSTRUCTION_ADD_HPP

#include "../instruction.hpp"

class AddInstruction : public Instruction {
public:
    AddInstruction(short rs, short rd, Section* section) : Instruction(), rs(rs), rd(rd), section(section) {}
    ~AddInstruction() override {};

    static Instruction* createInstruction(short, short, uint32, std::string, Section*, std::unordered_map<std::string, Symbol*>&, short);

    int write_section_data() override;

private:
    short rs;
    short rd;
    Section* section;
};

#endif