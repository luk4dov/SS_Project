#ifndef INSTRUCTION_SHL_HPP
#define INSTRUCTION_SHL_HPP

#include "../instruction.hpp"

class ShlInstruction : public Instruction {
public:
    ShlInstruction(short rs, short rd, Section* section);
    ~ShlInstruction() override {};

    static Instruction* createInstruction(short, short, uint32, std::string, Section*, std::unordered_map<std::string, Symbol*>&, short);

    int write_section_data() override;

private:
    short rs;
    short rd;
    Section* section;
};

#endif