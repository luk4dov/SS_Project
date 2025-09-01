#ifndef INSTRUCTION_INT_HPP
#define INSTRUCTION_INT_HPP

#include "../instruction.hpp"

class IntInstruction : public Instruction {
public:
    IntInstruction() : Instruction("int") {}
    ~IntInstruction() override {};

    static Instruction* createInstruction(const std::string&, int, int, uint32, const std::string&, int);

    int writeSectionData(Section*, std::unordered_map<std::string, Symbol*>&) override;
};


#endif