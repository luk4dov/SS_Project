#include "./binaryrw.hpp"

void BinaryRW::read(const std::string& fileName, std::unordered_map<std::string, Section*>& sectionTable, std::unordered_map<std::string, Symbol*>& symbolTable) {
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

            for(uint32 k = 0; k < vectorSize; k++) {
                offset.push_back(readUint32());
            }
            relocTable[symbol] = offset;
        }

        uint32 dataSize = readUint32();
        std::vector<uchar> sectionData;

        for(uint32 j = 0; j < dataSize; j ++) {
            sectionData.push_back(readByte());
        }

        sectionTable[section] = new Section(addr, section);
        sectionTable[section]->reloc_table = relocTable;
        sectionTable[section]->data = sectionData;
    }

    file.close();
}

void BinaryRW::write(const std::string& fileName, std::unordered_map<std::string, Section*>& sectionTable, std::unordered_map<std::string, Symbol*>& symbolTable) {
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


void BinaryRW::writeHex(const std::string& fileName, std::unordered_map<std::string, Section*>& sectionTable, uint32 maxSectionAddress) {
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

    // merge sections after maxSectionAddress into one file for readability
    // pickup sections after maxSectionAddress
    std::string maxSectionName;
    std::unordered_map<uint32, Section*> sectionsAfterMax = {};
    for(auto& [sectionName, section] : sectionTable) {
        if(section->addr > maxSectionAddress) {
            sectionsAfterMax[section->addr] = section;
        }
        if(section->addr == maxSectionAddress) {
            maxSectionName = sectionName;
        }
    }
    // merge sections from sectionsAfterMax into one section starting from maxSectionAddress + size of section with maxSectionAddress
    // starting from address closest to maxSectionAddress
    while(sectionsAfterMax.size() > 0) {
        uint32 currentAddress = maxSectionAddress + sectionTable[maxSectionName]->data.size();

        for(auto& [addr, section] : sectionsAfterMax) {
            if(addr == currentAddress) {
                sectionTable[maxSectionName]->data.insert(sectionTable[maxSectionName]->data.end(), section->data.begin(), section->data.end());
                currentAddress += section->data.size();
                sectionTable.erase(section->name);
                sectionsAfterMax.erase(addr);
                break;
            }
        }
    }

    // doesn't have to be in binary format, just for readability
    // doesn't even have to be connected to the binaryRW class
    file = std::fstream("program.txt", std::ios::out | std::ios::trunc);
    for(auto [sectionName, section] : sectionTable) {
        uint32 addr = section->addr;

        // skip sections after maxSectionAddress
        if(addr > maxSectionAddress) continue;

        for(uint32 i = 0; i < section->data.size(); i += 8) {
            file << std::hex << std::setw(8) << std::setfill('0') << addr + i << ": ";
            for(uint32 j = 0; j < 8 && (i + j) < section->data.size(); ++j) {
                file << std::hex << std::setw(2) << std::setfill('0') << (int)section->data[i + j] << " ";
            }
            file << '\n';
        }
    }
    file.close();
}

void BinaryRW::readHex(const std::string& fileName, std::unordered_map<uint32, std::vector<uchar>>& mem) {
    file = std::fstream(fileName, std::ios::in | std::ios::binary);

    uint32 numOfSections = readUint32();

    for(uint32 i = 0; i < numOfSections; ++i) {
        uint32 addr = readUint32();
        uint32 size = readUint32();

        // 32 bits for address -> 20 bits for block and 12 for word
        for(uint32 blockAddress = addr; blockAddress < addr + size;) {
            uint32 block = blockAddress >> 12;

            if(mem.find(block) == mem.end()) {
                mem[block] = std::vector<uchar>(1<<12, 0);
            }

            for(uint32 word = blockAddress & 0xfff; word < (1 << 12) && (block << 12 | word) < addr + size * sizeof(uchar); ++word) {
                mem[block][word] = readByte();
            }

            blockAddress += (1 << 12);
        }
    }

    file.close();
}
