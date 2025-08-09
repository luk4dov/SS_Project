#ifndef INSTRUCTION_XCHG_HPP
#define INSTRUCTION_XCHG_HPP

#include "../instruction.hpp"

class XchgInstruction : public Instruction {
public:
    XchgInstruction(short rs, short rd, Section* section) : Instruction(), rs(rs), rd(rd), section(section) {}
    ~XchgInstruction() override {}

    static Instruction* createInstruction(short, short, uint32, std::string, Section*, std::unordered_map<std::string, Symbol*>&, short);
    
    int write_section_data() override;
private:
    short rs;
    short rd;
    Section* section;
};

#endif