#ifndef ASSEMBLER_HPP
#define ASSEMBLER_HPP

#include "../common/instruction.hpp"
#include "../common/binaryrw.hpp"
#include "../common/exceptions.hpp"

class Assembler {
public: 
    Assembler(const char* inputFile, const char* outputFile) : inputFile(inputFile), outputFile(outputFile), section("") { 
        sectionTable = {}; symbolTable = {}; 
    }

    ~Assembler();

    int assemble();
    void write();

    void selectInstruction(std::string, int, int, uint32, std::string, int);
    void labelDefinition(std::string);
    void selectDirective(std::string, std::string, uint32);

    void printStat();
    
private:

    const char* inputFile;
    const char* outputFile;

    std::string section;
    std::unordered_map<std::string, Section*> sectionTable;
    std::unordered_map<std::string, Symbol*> symbolTable;

    void removeLocalSymbols();
};

#endif