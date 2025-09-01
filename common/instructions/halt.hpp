#ifndef INSTRUCTION_HALT_HPP
#define INSTRUCTION_HALT_HPP

#include "../instruction.hpp"

class HaltInstruction : public Instruction {
public:
    HaltInstruction() : Instruction("halt"){}
    ~HaltInstruction() override {};

    static Instruction* createInstruction(const std::string&, int, int, uint32, const std::string&, int);

    int writeSectionData(Section*, std::unordered_map<std::string, Symbol*>&) override;
};


#endif