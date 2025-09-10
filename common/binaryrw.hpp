#ifndef BINARY_RW_HPP
#define BINARY_RW_HPP

#include "types.hpp"
#include <fstream>

class BinaryRW {
public:
    BinaryRW() {}
    ~BinaryRW() {}

    void read(const std::string&, std::unordered_map<std::string, Section*>& sectionTable, std::unordered_map<std::string, Symbol*>& symbolTable);
    void write(const std::string&, std::unordered_map<std::string, Section*>& sectionTable, std::unordered_map<std::string, Symbol*>& symbolTable);


    void writeHex(const std::string&, std::unordered_map<std::string, Section*>&, uint32);
    void readHex(const std::string&, std::unordered_map<uint32, std::vector<uchar>>&);

private:
    std::fstream file;

    uint32 readUint32();
    std::string readString();
    uchar readByte();

    void writeUint32(uint32);
    void writeString(std::string);
    void writeByte(uchar);
};

#endif