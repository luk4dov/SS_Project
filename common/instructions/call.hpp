#ifndef INSTRUCTION_CALL_HPP
#define INSTRUCTION_CALL_HPP

#include "../instruction.hpp"

class CallInstruction : public Instruction {
public:
    CallInstruction(std::string label, uint32 immediate, Section* section, std::unordered_map<std::string, Symbol*>& symbolTable, short type) : Instruction(), label(label), immediate(immediate), section(section), symbolTable(symbolTable), type(type) {}
    ~CallInstruction() override {};

    static Instruction* createInstruction(short, short, uint32, std::string, Section*, std::unordered_map<std::string, Symbol*>&, short);

    int write_section_data() override;

private:
    std::string label;
    uint32 immediate;
    Section* section;
    std::unordered_map<std::string, Symbol*>& symbolTable;
    short type;
};

#endif