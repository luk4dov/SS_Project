#ifndef INSTRUCTION_HPP
#define INSTRUCTION_HPP

#include "types.hpp"


class Instruction {
public:    
    Instruction(std::string mn) : mn(mn) {}
    virtual ~Instruction() {};
    
    Instruction* pickInstruction(const std::string&, int, int, uint32, const std::string&, int);

    virtual int writeSectionData(Section*, std::unordered_map<std::string, Symbol*>&) = 0;

    uint32 serialize(OperationCode, short, short, short, short, short);

    void write_binary(Section*, uint32);

    static std::unordered_map<std::string, Instruction* (*) (const std::string&, int, int, uint32, const std::string&, int)> instructions;

protected:
    std::string mn;
};

#endif