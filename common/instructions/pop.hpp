#ifndef INSTRUCTION_POP_HPP
#define INSTRUCTION_POP_HPP

#include "../instruction.hpp"

class PopInstruction : public Instruction {
public:
    PopInstruction(short, Section*, short);
    ~PopInstruction() {};

    static Instruction* createInstruction(short, short, uint32, std::string, Section*, std::unordered_map<std::string, Symbol*>&, short);
    
    int write_section_data() override;

private:
    short rd;
    Section* section;
    short type;
};

#endif