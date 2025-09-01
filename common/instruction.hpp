#ifndef INSTRUCTION_HPP
#define INSTRUCTION_HPP

#include "types.hpp"

class Instruction {
public:    
    Instruction(std::string mn) : mn(mn) {}
    virtual ~Instruction() {};

    virtual int writeSectionData(Section*, std::unordered_map<std::string, Symbol*>&) = 0;

    static std::unordered_map<std::string, Instruction* (*) (const std::string&, int, int, uint32, const std::string&, int)> parsedInstructions;
    static std::unordered_map<OperationCode, Instruction* (*) (int, int, int, int, int)> binaryInstructions;

protected:
    std::string mn;

    uint32 serialize(OperationCode, short, short, short, short, short);
    void write_binary(Section*, uint32);
};

#endif