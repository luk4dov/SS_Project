#ifndef INSTRUCTION_RET_HPP
#define INSTRUCTION_RET_HPP

#include "../instruction.hpp"

class RetInstruction : public Instruction {
public:
    RetInstruction(Section* section) : Instruction(), section(section) {}
    ~RetInstruction() override {}

    static Instruction* createInstruction(short, short, uint32, std::string, Section*, std::unordered_map<std::string, Symbol*>&, short);

    int write_section_data() override;
private:
    Section* section;
};

#endif