#include "binaryrw.hpp"

void BinaryRW::read(std::string fileName) {
    file = std::fstream(fileName, std::ios::in | std::ios::binary);

    uint32 symbolTableSize = readUint32();
    
    for(uint32 i = 0; i < symbolTableSize; ++i) {
        std::string symbolName = readString();
        std::string section = readString();
        uint32 offset = readUint32();
        bool global = readByte();
        bool defined = readByte();
        symbolTable[symbolName] = new Symbol(section, offset, global, defined);
    }

    uint32 sectionTableSize = readUint32();
    
    for(uint32 i = 0; i < sectionTableSize; ++i) {
        std::string section = readString(); // section name
        uint32 addr = readUint32(); // section address
        uint32 relocTableSize = readUint32(); // size of reloc table

        std::unordered_map<std::string, std::vector<uint32>> relocTable; // read reloc table
        for(uint32 j = 0; j < relocTableSize; j++) {
            std::string symbol = readString();
            uint32 vectorSize = readUint32();
            std::vector<uint32> offset;
            
            for(int k = 0; k < vectorSize; k++) {
                offset.push_back(readUint32());
            }
            relocTable[symbol] = offset;
        }
        
        uint32 dataSize = readUint32();
        std::vector<uchar> sectionData;

        for(uint32 j = 0; j < dataSize; j ++) {
            sectionData.push_back(readByte());
        }

        sectionTable[section] = new Section(addr, 0, section);
        sectionTable[section]->reloc_table = relocTable;
        sectionTable[section]->data = sectionData;
    }

    file.close();
}

void BinaryRW::write(std::string fileName) {
    file = std::fstream(fileName, std::ios::out | std::ios::trunc | std::ios::binary);
    
    writeUint32(symbolTable.size());
    
    for(auto [symbolName, symbol] : symbolTable) {
        writeString(symbolName);
        writeString(symbol->section);
        writeUint32(symbol->offset);
        writeByte(static_cast<uchar>(symbol->global));
        writeByte(static_cast<uchar>(symbol->defined));
    }

    writeUint32(sectionTable.size());
    for(auto [sectionName, section] : sectionTable) {
        writeString(sectionName); // section -> name
        writeUint32(section->addr); // section -> addr
        writeUint32(section->reloc_table.size());
        for(auto [symbol, reloc] : section->reloc_table) { // write reloc table
            writeString(symbol);
            writeUint32(reloc.size());
            for(uint32 offset : reloc) {
                writeUint32(offset);
            }
        }

        writeUint32(section->data.size());
        
        for(uchar byte : section->data) {
            writeByte(byte);
        }
    }
    file.close();
}

void BinaryRW::writeUint32(uint32 num) {
    std::vector<uchar> bytes(4,0);
    bytes[0] = num & 0xff;
    bytes[1] = (num >> 8) & 0xff;
    bytes[2] = (num >> 16) & 0xff;
    bytes[3] = (num >> 24) & 0xff;
    file.write(reinterpret_cast<const char*>(bytes.data()), 4);
}

void BinaryRW::writeString(std::string s) {
    uint32 size = s.size(); 
    writeUint32(size+1); // write the number of letters with '\0' sign
    std::vector<uchar> letters(size+1, 0);
    std::copy(s.begin(), s.end(), letters.begin());
    letters[size] = static_cast<uint8_t>('\0');
    file.write(reinterpret_cast<const char*>(letters.data()), letters.size());
}

void BinaryRW::writeByte(uchar data) {
    file.write(reinterpret_cast<char*>(&data), 1);
}

uint32 BinaryRW::readUint32() {
    uchar bytes[4];
    file.read(reinterpret_cast<char*>(bytes), 4);
    return (uint32(bytes[3]) << 24) |
           (uint32(bytes[2]) << 16) |
           (uint32(bytes[1]) << 8)  |
            uint32(bytes[0]);
}

std::string BinaryRW::readString() {
    uint32 length = readUint32();
    std::vector<char> characters(length, 0);
    file.read(characters.data(), length);
    return std::string(characters.data());
}

uchar BinaryRW::readByte() {
    uchar byte = 0;
    file.read(reinterpret_cast<char*>(&byte), 1);
    return byte;
}

void BinaryRW::writeHex(std::string fileName) {
    file = std::fstream(fileName, std::ios::out | std::ios::trunc | std::ios::binary);
    writeUint32(sectionTable.size());

    for(auto [sectionName, section] : sectionTable) {
        writeUint32(section->addr);
        writeUint32(section->data.size());
        for(uchar data : section->data) {
            writeByte(data);
        }
    }

    file.close();
    
    std::fstream file = std::fstream("program.txt", std::ios::out | std::ios::trunc);
    for(auto [sectionName, section] : sectionTable) {
        int addr = section->addr;

        for(int i = 0; i < section->data.size(); i += 8) {
            file << std::hex << std::setw(8) << std::setfill('0') << addr + i << ": ";
            for(int j = 0; j < 8 && (i + j) < section->data.size(); ++j) {
                file << std::hex << std::setw(2) << std::setfill('0') << (int)section->data[i + j] << " ";
            }
            file << '\n';
        }
    }
    file.close();
}