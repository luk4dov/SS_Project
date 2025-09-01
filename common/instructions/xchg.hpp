#ifndef INSTRUCTION_XCHG_HPP
#define INSTRUCTION_XCHG_HPP

#include "../instruction.hpp"

class XchgInstruction : public Instruction {
public:
    XchgInstruction(int r1, int r2, int r3, int disp) : Instruction("xchg"), r1(r1), r2(r2), r3(r3), disp(disp) {}
    ~XchgInstruction() override {}

    static Instruction* parsedInstruction(const std::string&, int, int, uint32, const std::string&, int);
    static Instruction* binaryInstruction(int, int, int, int, int);

    int writeSectionData(Section*, std::unordered_map<std::string, Symbol*>&) override;
private:
    int r1;
    int r2;
    int r3;
    int disp;
};

#endif