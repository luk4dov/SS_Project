#ifndef INSTRUCTION_IRET_HPP
#define INSTRUCTION_IRET_HPP

#include "../instruction.hpp"

class IretInstruction : public Instruction {
public:
    IretInstruction(Section* section) : Instruction(), section(section) {}
    ~IretInstruction() override {};

    static Instruction* createInstruction(short, short, uint32, std::string, Section*, std::unordered_map<std::string, Symbol*>&, short);

    int write_section_data() override;
private:
    Section* section;
};

#endif