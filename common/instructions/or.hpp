#ifndef INSTRUCTION_OR_HPP
#define INSTRUCTION_OR_HPP

#include "../instruction.hpp"

class OrInstruction : public Instruction {
public:
    OrInstruction(short rs, short rd, Section* section) : Instruction(), rs(rs), rd(rd), section(section) {}
    ~OrInstruction() override {};

    static Instruction* createInstruction(short, short, uint32, std::string, Section*, std::unordered_map<std::string, Symbol*>&, short);

    int write_section_data() override;

private:
    short rs;
    short rd;
    Section* section;
};

#endif