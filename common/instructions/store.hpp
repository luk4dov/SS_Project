#ifndef INSTRUCTION_STORE_HPP
#define INSTRUCTION_STORE_HPP

#include "../instruction.hpp"

class StoreInstruction : public Instruction {
public:
    StoreInstruction(int, int, uint32, const std::string&, int, LoadStoreOC);
    StoreInstruction(int, int, int, int, int);
    ~StoreInstruction() override {};

    static Instruction* parsedInstruction(const std::string&, int, int, uint32, const std::string&, int);
    static Instruction* binaryInstruction(int, int, int, int, int);

    int writeSectionData(Section*, std::unordered_map<std::string, Symbol*>&) override;
    void execute(CPU*) override;

private:
    int r1;
    int r2;
    int r3;
    int disp;
    uint32 immediate;
    std::string label;
    int type;
    LoadStoreOC op;
    int mod;
};

#endif
