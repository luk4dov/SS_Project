#ifndef INSTRUCTION_PUSH_HPP
#define INSTRUCTION_PUSH_HPP

#include "../instruction.hpp"

class PushInstruction : public Instruction {
public:
    PushInstruction(short, Section*);
    ~PushInstruction() {};

    static Instruction* createInstruction(short, short, uint32, std::string, Section*, std::unordered_map<std::string, Symbol*>&, short);
    
    int write_section_data() override;

private:
    short rs;
    Section* section;
};

#endif