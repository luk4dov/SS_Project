#ifndef INSTRUCTION_STORE_HPP
#define INSTRUCTION_STORE_HPP

#include "../instruction.hpp"

class StoreInstruction : public Instruction {
public:
    StoreInstruction(short, short, uint32, std::string, Section*, std::unordered_map<std::string, Symbol*>&, short);
    ~StoreInstruction() override {};

    static Instruction* createInstruction(short, short, uint32, std::string, Section*, std::unordered_map<std::string, Symbol*>&, short);

    int write_section_data() override;

private:
    short rs;
    short rd;
    uint32 immediate;
    std::string label;
    Section* section;
    std::unordered_map<std::string, Symbol*>& symbolTable;
    short type;
};

#endif