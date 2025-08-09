#ifndef INSTRUCTION_HPP
#define INSTRUCTION_HPP

#include "types.hpp"


class Instruction {
public:    
    Instruction() {}
    virtual ~Instruction() {};
    
    Instruction* pickInstruction(std::string, short, short, uint32, std::string, Section*, std::unordered_map<std::string, Symbol*>&, short type);

    virtual int write_section_data() = 0;

    uint32 serialize(OperationCode, short, short, short, short, short);

    void write_binary(Section*, uint32);

    static std::unordered_map<std::string, Instruction* (*) (short, short, uint32, std::string, Section*, std::unordered_map<std::string, Symbol*>&, short)> instructions;
};

#endif