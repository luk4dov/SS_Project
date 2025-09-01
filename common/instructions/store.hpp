#ifndef INSTRUCTION_STORE_HPP
#define INSTRUCTION_STORE_HPP

#include "../instruction.hpp"

class StoreInstruction : public Instruction {
public:
    StoreInstruction(int, int, uint32, const std::string&, int, LoadStoreOC);
    ~StoreInstruction() override {};

    static Instruction* createInstruction(const std::string&, int, int, uint32, const std::string&, int);

    int writeSectionData(Section*, std::unordered_map<std::string, Symbol*>&) override;

private:
    int r1;
    int r2;
    int r3;
    int disp;
    uint32 immediate;
    std::string label;
    short type;
    LoadStoreOC op;
};

#endif
