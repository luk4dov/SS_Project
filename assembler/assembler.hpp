#ifndef ASSEMBLER_HPP
#define ASSEMBLER_HPP

#include "../common/instruction.hpp"
#include "../common/binaryrw.hpp"
#include "../common/exceptions.hpp"

class Assembler {
public: 
    Assembler(const char* inputFile, const char* outputFile) : inputFile(inputFile), outputFile(outputFile), section("") { 
        sectionTable = {}; symbolTable = {}; rw = new BinaryRW(); unresolvedEqus = {}; // literalPool = {};
    }

    ~Assembler();

    int assemble();
    void write();

    void selectInstruction(const std::string&, int, int, uint32, const std::string&, int);
    void labelDefinition(const std::string&);
    void selectDirective(const std::string&, const std::string&, uint32);
    void equDirective(uint32, const std::string&, const std::string&, const std::string&, uint32, uint32, uint32);

private:

    BinaryRW* rw;

    const char* inputFile;
    const char* outputFile;

    std::string section;
    std::unordered_map<std::string, Section*> sectionTable;
    std::unordered_map<std::string, Symbol*> symbolTable;

    std::queue<EquExpression*> unresolvedEqus;

    
    void resolveEqus();
    int tryToResolve(EquExpression*);
    
    void removeLocalSymbols();
    void printStat();
};

#endif