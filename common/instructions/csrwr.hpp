#ifndef INSTRUCTION_CSRWR_HPP
#define INSTRUCTION_CSRWR_HPP

#include "../instruction.hpp"

class CsrwrInstruction : public Instruction {
public:    
    CsrwrInstruction(short rs, std::string csr, Section* section) : Instruction(),  rs(rs), csr(csr), section(section) {}
    ~CsrwrInstruction() override {}

    static Instruction* createInstruction(short reg1, short reg2, uint32 immediate, std::string label, Section* section, std::unordered_map<std::string, Symbol*>& symbolTable, short mn);

    int write_section_data() override;

private:
    short rs;
    std::string csr;
    Section* section;
};

#endif