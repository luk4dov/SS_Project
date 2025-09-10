#ifndef LINKER_HPP
#define LINKER_HPP

#include "../common/binaryrw.hpp"
#include "../common/exceptions.hpp"

class Linker {

public:
    Linker(std::vector<std::string> inputFiles, const std::string& outputFile, bool relocatable, std::unordered_map<std::string, uint32> sectionAddress, uint32 maxSectionAddress, const std::string& maxSectionName) :
        inputFiles(inputFiles), outputFile(outputFile), relocatable(relocatable), sectionAddress(sectionAddress), maxSectionAddress(maxSectionAddress), maxSectionName(maxSectionName) {
            sectionTable = {};
            symbolTable = {};
            rw = new BinaryRW();
        }
    ~Linker();

    int linkAndCreateOutput();
    
    int link();
    int createRelocatable(std::string outputFile = "linkerRelocatable.o");
    int createExecutable(std::string outputFile = "linkerExecutable.hex");
        
private:

    BinaryRW* rw;

    std::unordered_map<std::string, Section*> sectionTable;
    std::unordered_map<std::string, Symbol*> symbolTable;

    std::vector<std::string> inputFiles;
    std::string outputFile;

    bool relocatable;
    
    std::unordered_map<std::string, uint32> sectionAddress;
    uint32 maxSectionAddress;
    std::string maxSectionName;

    void printStat();
};

#endif