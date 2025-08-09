#ifndef INSTRUCTION_CSRRD_HPP
#define INSTRUCTION_CSRRD_HPP

#include "../instruction.hpp"

class CsrrdInstruction : public Instruction {
public:    
    CsrrdInstruction(short rd, std::string csr, Section* section) : Instruction(), rd(rd), csr(csr), section(section) {}
    ~CsrrdInstruction() override {}

    static Instruction* createInstruction(short reg1, short reg2, uint32 immediate, std::string label, Section* section, std::unordered_map<std::string, Symbol*>& symbolTable, short mn);

    int write_section_data() override;

private:
    short rd;
    std::string csr;
    Section* section;
};

#endif