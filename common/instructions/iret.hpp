#ifndef INSTRUCTION_IRET_HPP
#define INSTRUCTION_IRET_HPP

#include "../instruction.hpp"

class IretInstruction : public Instruction {
public:
    IretInstruction() : Instruction("iret") {}
    ~IretInstruction() override {};

    static Instruction* parsedInstruction(const std::string&, int, int, uint32, const std::string&, int);
    static Instruction* binaryInstruction(int, int, int, int, int);

    int writeSectionData(Section*, std::unordered_map<std::string, Symbol*>&) override;
    void execute(CPU*) override;
};

#endif