#ifndef INSTRUCTION_XOR_HPP
#define INSTRUCTION_XOR_HPP

#include "../instruction.hpp"

class XorInstruction : public Instruction {
public:
    XorInstruction(short rs, short rd, Section* section) : Instruction(), rs(rs), rd(rd), section(section) {}
    ~XorInstruction() override {};

    static Instruction* createInstruction(short, short, uint32, std::string, Section*, std::unordered_map<std::string, Symbol*>&, short);

    int write_section_data() override;

private:
    short rs;
    short rd;
    Section* section;
};

#endif