#ifndef INSTRUCTION_BGT_HPP
#define INSTRUCTION_BGT_HPP

#include "../instruction.hpp"

class BgtInstruction : public Instruction {
public:    
    BgtInstruction(short, short, uint32, std::string, Section*,  std::unordered_map<std::string, Symbol*>, short, JMPCondition);
    ~BgtInstruction() override {}
    
    static Instruction* createInstruction(short, short, uint32, std::string, Section*, std::unordered_map<std::string, Symbol*>&, short);
    
    int write_section_data() override;

private:
    short r1;
    short r2;
    uint32 immediate;
    std::string label;
    Section* section;
    std::unordered_map<std::string, Symbol*> symbolTable;
    short type;
    JMPCondition cond;
};

#endif