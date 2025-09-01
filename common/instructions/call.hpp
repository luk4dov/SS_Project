#ifndef INSTRUCTION_CALL_HPP
#define INSTRUCTION_CALL_HPP

#include "../instruction.hpp"

class CallInstruction : public Instruction {
public:
    CallInstruction(const std::string& label, uint32 immediate, int type) : Instruction("call"), label(label), immediate(immediate), type(type) {}
    ~CallInstruction() override {};

    static Instruction* createInstruction(const std::string&, int, int, uint32, const std::string&, int);

    int writeSectionData(Section*, std::unordered_map<std::string, Symbol*>&) override;

private:
    std::string label;
    uint32 immediate;
    int type;
};

#endif