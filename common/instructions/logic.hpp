#ifndef LOGIC_HPP
#define LOGIC_HPP

#include "instruction.hpp"

class LogicInstruction : public Instruction {
public:
    LogicInstruction(int r1, int r2, int r3, LogicOC mod) : Instruction("logic"), r1(r1), r2(r2), r3(r3), mod(mod)
    {}

    ~LogicInstruction() {}

    static Instruction* parsedInstruction(const std::string&, int, int, uint32, const std::string&, int);
    static Instruction* binaryInstruction(int, int, int, int, int);

    int writeSectionData(Section*, std::unordered_map<std::string, Symbol*>&) override;

private:
    int r1;
    int r2;
    int r3;
    LogicOC mod;
};

#endif