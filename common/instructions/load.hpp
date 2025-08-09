#ifndef INSTRUCTION_LOAD_HPP
#define INSTRUCTION_LOAD_HPP

#include "../instruction.hpp"

class LoadInstruction : public Instruction {
public:
    LoadInstruction(short, short, uint32, std::string, Section*, std::unordered_map<std::string, Symbol*>&, short);
    ~LoadInstruction() override {};

    static Instruction* createInstruction(short, short, uint32, std::string, Section*, std::unordered_map<std::string, Symbol*>&, short);

    int write_section_data() override;

private:
    short rd;
    short rs;
    uint32 immediate;
    std::string label;
    short type;
    Section* section;
    std::unordered_map<std::string, Symbol*> symbolTable;
};

#endif