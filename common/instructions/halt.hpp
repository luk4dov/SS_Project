#ifndef INSTRUCTION_HALT_HPP
#define INSTRUCTION_HALT_HPP

#include "../instruction.hpp"

class HaltInstruction : public Instruction {
public:
    HaltInstruction(Section* section) : Instruction(), section(section) {}
    ~HaltInstruction() override {};

    static Instruction* createInstruction(short, short, uint32, std::string, Section*, std::unordered_map<std::string, Symbol*>&, short);

    int write_section_data() override;
private:
    Section* section;
};


#endif