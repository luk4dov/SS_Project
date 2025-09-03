#ifndef SHIFT_HPP
#define SHIFT_HPP

#include "instruction.hpp"

class ShiftInstruction : public Instruction {
public:
    ShiftInstruction(int r1, int r2, int r3, ShiftOC mod) : Instruction("shift"), r1(r1), r2(r2), r3(r3), mod(mod)
    {}

    ~ShiftInstruction() {}

    static Instruction* parsedInstruction(const std::string&, int, int, uint32, const std::string&, int);
    static Instruction* binaryInstruction(int, int, int, int, int);

    int writeSectionData(Section*, std::unordered_map<std::string, Symbol*>&) override;
    void execute(CPU*) override;

private:
    int r1;
    int r2;
    int r3;
    ShiftOC mod;
};

#endif