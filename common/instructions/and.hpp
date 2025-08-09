#ifndef INSTRUCTION_AND_HPP
#define INSTRUCTION_AND_HPP

#include "../instruction.hpp"

class AndInstruction : public Instruction {
public:
    AndInstruction(short rs, short rd, Section* section) : Instruction(), rs(rd), rd(rd), section(section) {}
    ~AndInstruction() override {};

    static Instruction* createInstruction(short, short, uint32, std::string, Section*, std::unordered_map<std::string, Symbol*>&, short);

    int write_section_data() override;

private:
    short rs;
    short rd;
    Section* section;
};

#endif