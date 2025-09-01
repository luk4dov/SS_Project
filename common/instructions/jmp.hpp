#ifndef INSTRUCTION_JMP_HPP
#define INSTRUCTION_JMP_HPP

#include "../instruction.hpp"

class JmpInstruction : public Instruction {
public:    
    JmpInstruction(int, int, uint32, const std::string&, int, JMPCondition);
    ~JmpInstruction() override {}

    static Instruction* createInstruction(const std::string&, int, int, uint32, const std::string&, int);

    int writeSectionData(Section*, std::unordered_map<std::string, Symbol*>&) override;

private:
    int r1;
    int r2;
    int r3;
    uint32 immediate;
    std::string label;
    int type;
    JMPCondition cond;
};

#endif