#ifndef INSTRUCTION_CALL_HPP
#define INSTRUCTION_CALL_HPP

#include "../instruction.hpp"

class CallInstruction : public Instruction {
public:
    CallInstruction(const std::string& label, uint32 immediate, int type) : Instruction("call"), label(label), immediate(immediate), type(type) {}
    CallInstruction(int mod, int r1, int r2, int r3, int disp) : Instruction("call"), r1(r1), r2(r2), r3(r3), disp(disp), mod(mod) {}
    ~CallInstruction() override {};

    static Instruction* parsedInstruction(const std::string&, int, int, uint32, const std::string&, int);
    static Instruction* binaryInstruction(int, int, int, int, int);

    int writeSectionData(Section*, std::unordered_map<std::string, Symbol*>&) override;
    void execute(CPU*) override;

private:
    int r1;
    int r2;
    int r3;
    int disp;
    int mod;
    std::string label;
    uint32 immediate;
    int type;
};

#endif