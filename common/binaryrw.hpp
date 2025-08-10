#ifndef BINARY_RW_HPP
#define BINARY_RW_HPP

#include "types.hpp"
#include <fstream>

class BinaryRW {
public:
    BinaryRW(std::unordered_map<std::string, Section*>& sectionTable, std::unordered_map<std::string, Symbol*>& symbolTable) : 
            symbolTable(symbolTable), sectionTable(sectionTable) {}
    ~BinaryRW() {}

    void read(std::string);
    void write(std::string);
private:
    std::fstream file;

    std::unordered_map<std::string, Section*>& sectionTable;
    std::unordered_map<std::string, Symbol*>& symbolTable;

    uint32 readUint32();
    std::string readString();
    char readByte();

    void writeUint32(uint32);
    void writeString(std::string);
    void writeByte(char);
};

#endif