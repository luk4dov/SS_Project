#ifndef INSTRUCTION_SHR_HPP
#define INSTRUCTION_SHR_HPP

#include "../instruction.hpp"

class ShrInstruction : public Instruction {
public:
    ShrInstruction(short rs, short rd, Section* section) : Instruction(), rs(rs), rd(rd), section(section) {}
    ~ShrInstruction() override {};

    static Instruction* createInstruction(short, short, uint32, std::string, Section*, std::unordered_map<std::string, Symbol*>&, short);

    int write_section_data() override;

private:
    short rs;
    short rd;
    Section* section;
};

#endif