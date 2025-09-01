#ifndef INSTRUCTION_LOAD_HPP
#define INSTRUCTION_LOAD_HPP

#include "../instruction.hpp"

class LoadInstruction : public Instruction {
public:
    LoadInstruction(int, int, uint32, const std::string&, int, LoadStoreOC);
    ~LoadInstruction() override {};

    static Instruction* createInstruction(const std::string&, int, int, uint32, const std::string&, int);

    int writeSectionData(Section*, std::unordered_map<std::string, Symbol*>&) override;

private:
    int r1;
    int r2;
    int r3;
    int disp;
    uint32 immediate;
    std::string label;
    int type;
    LoadStoreOC op;
};

#endif