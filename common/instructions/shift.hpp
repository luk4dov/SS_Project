#ifndef SHIFT_HPP
#define SHIFT_HPP

#include "instruction.hpp"

class ShiftInstruction : public Instruction {
public:
    ShiftInstruction(int r1, int r2, int r3, ShiftOC mod) : Instruction("shift"), r1(r1), r2(r2), r3(r3), mod(mod)
    {}

    ~ShiftInstruction() {}

    static Instruction* createInstruction(const std::string&, int, int, uint32, const std::string&, int);

    int writeSectionData(Section*, std::unordered_map<std::string, Symbol*>&) override;

private:
    int r1;
    int r2;
    int r3;
    ShiftOC mod;
};

#endif