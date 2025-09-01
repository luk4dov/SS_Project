#ifndef INSTRUCTION_IRET_HPP
#define INSTRUCTION_IRET_HPP

#include "../instruction.hpp"

class IretInstruction : public Instruction {
public:
    IretInstruction() : Instruction("iret") {}
    ~IretInstruction() override {};

    static Instruction* createInstruction(const std::string&, int, int, uint32, const std::string&, int);

    int writeSectionData(Section*, std::unordered_map<std::string, Symbol*>&) override;
};

#endif