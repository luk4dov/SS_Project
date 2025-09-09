#include "linker.hpp"

Linker::~Linker() {
    for(auto& [sectionName, section] : sectionTable) {
        delete section;
    }
    for(auto& [symbolName, symbol] : symbolTable) {
        delete symbol;
    }
}

int Linker::link() {
    std::unordered_map<std::string, Section*> tempSectionTable = {};
    std::unordered_map<std::string, Symbol*> tempSymbolTable = {};

    BinaryRW* rw = new BinaryRW();
    for(std::string file : inputFiles) {
        rw->read(file, tempSectionTable, tempSymbolTable);

        std::unordered_map<std::string, uint32> sectionSize = {};

        // merge sections and symbols from temp tables into main table      
        for(auto& [tempSectionName, tempSection] : tempSectionTable) {
            if (sectionTable.find(tempSectionName) == sectionTable.end()) { // insert it as new section
                sectionTable[tempSectionName] = tempSection; 
                continue;
            }

            sectionSize[tempSectionName] = sectionTable[tempSectionName]->data.size();
            // section with that name already exists, merge it into one and update relocation table

            // loop through all the relocations in current section and add size of the section that's already in big section table to the offsets
            // relocation of local symbols (because it's done over sectio's name, and not symbol's name)
            for(auto& [symbolName, offsets] : tempSection->reloc_table) {
                for(auto offset : offsets) {
                    sectionTable[tempSectionName]->reloc_table[symbolName].push_back(offset + sectionTable[tempSectionName]->data.size());
                }
            }

            // copy data from temp section the section that already exists
            sectionTable[tempSectionName]->data.insert(sectionTable[tempSectionName]->data.end(), tempSection->data.begin(), tempSection->data.end());
        }

        for(auto& [tempSymbolName, tempSymbol] : tempSymbolTable) {
            if (symbolTable.find(tempSymbolName) == symbolTable.end()) { // insert it as new symbol
                symbolTable[tempSymbolName] = tempSymbol;
                symbolTable[tempSymbolName]->offset += sectionSize[tempSymbol->section]; // adjust offset to the new section
                continue;
            }
            // symbol with that name already exists, check if it's defined or not
            // if it's defined, continue
            // if it's not defined, check if the tempSymbol is defined, if it is, then update the symbol in the main table
            if(symbolTable[tempSymbolName]->defined && tempSymbolName[0] != '.') {
                if(tempSymbol->defined) {  // two global symbols with the same name and both defined, error
                    throw SymbolMultipleDefinition(tempSymbolName);
                }
            } else if(tempSymbol->defined) { // we found definition of symbol, update symbol table
                Symbol* old = symbolTable[tempSymbolName];
                symbolTable[tempSymbolName] = tempSymbol;
                symbolTable[tempSymbolName]->offset += sectionTable[tempSymbol->section]->data.size() - tempSectionTable[tempSymbol->section]->data.size(); // adjust offset to the new section
                delete old;
            }
        }

        tempSectionTable.clear();
        tempSymbolTable.clear();
    }

    return 0;
}

int Linker::createRelocatable(std::string outputFile) {
    BinaryRW* rw = new BinaryRW();
    rw->write(outputFile, sectionTable, symbolTable);
    delete rw;
    return 0;
}

int Linker::createExecutable(std::string outputFile) {
    for(const auto& [symbolName, symbol] : symbolTable) {
        if(!symbol->defined) {
            throw SymbolNotDefined(symbolName);
        }
    }

    for(auto& [sectionName, startingAddress] : sectionAddress) {
        if(sectionTable.find(sectionName) == sectionTable.end()) {
            throw SectionNotFound(sectionName);
        }

        sectionTable[sectionName]->addr = startingAddress;
        // go through all sections and check if they overlap

        for(auto& [otherSectionName, otherSection] : sectionTable) {
            if(otherSectionName == sectionName) continue; // skip self

            // check if sections overlap
            if((sectionTable[sectionName]->addr >= otherSection->addr && sectionTable[sectionName]->addr < otherSection->addr + otherSection->data.size()) ||
               (otherSection->addr >= sectionTable[sectionName]->addr && otherSection->addr < sectionTable[sectionName]->addr + sectionTable[sectionName]->data.size())) {
                throw SectionOverlap(sectionName, otherSectionName);
            }
        }
    }

    // after checking overlaps, it's time to place remaining sections behind the one with the biggest address
    uint32 currentAddress = maxSectionAddress + sectionTable[maxSectionName]->data.size();
    for(auto& [sectionName, section] : sectionTable) {
        if(sectionAddress.find(sectionName) != sectionAddress.end()) { // it's placed by -place option
            continue;
        }
        section->addr = currentAddress;
        currentAddress += section->data.size();
    }

    // resolve relocations
    for(const auto& [sectionName, section] : sectionTable) {
        for(const auto& [symbolName, offsets] : section->reloc_table) {
            Symbol* symbol = symbolTable[symbolName];
            uint32 symbolAddress = sectionTable[symbol->section]->addr + symbol->offset;
            
            for(uint32 offset : offsets) {
                uint32 addend = (section->data[offset] & 0xff) | 
                                 ((section->data[offset+1] & 0xff) << 8) | 
                                 ((section->data[offset+2] & 0xff) << 16) | 
                                 ((section->data[offset+3] & 0xff) << 24);

                section->data[offset] = (symbolAddress + addend) & 0xff;
                section->data[offset+1] = ((symbolAddress + addend) >> 8) & 0xff;
                section->data[offset+2] = ((symbolAddress + addend) >> 16) & 0xff;
                section->data[offset+3] = ((symbolAddress + addend) >> 24) & 0xff;
            }
        }
    }

    // write to binary file
    BinaryRW* rw = new BinaryRW();
    rw->writeHex(outputFile, sectionTable, maxSectionAddress);
    delete rw;
    return 0;
}

int Linker::linkAndCreateOutput() {
    try {
        link();

        if(relocatable) {
            return createRelocatable(outputFile);
        }
        else {
            return createExecutable(outputFile);
        }

        return 0;
    }
    catch (SymbolNotDefined e) { return -12; }
    catch (SectionNotFound e) { return -13; }
    catch (SectionOverlap e) { return -14; }
    catch (SymbolMultipleDefinition e) { return -15; }
}

void Linker::printStat() {
    std::cout << "Symbol table: symbol  |  section  |  offset  |  defined  |  bind \n";
    for(auto [symbolName, symbol] : symbolTable) {
        std::cout << symbolName << " | " << symbol->section << " | " << std::hex << symbol->offset << " | " << symbol->defined << " | " << symbol->global << '\n';
    }
    std::cout << "------------------------------------------------------\n";
    std::cout << "Section table: name  |  addr  |  size \n";
    for(auto [sectionName, section] : sectionTable) {
        std::cout << sectionName << " | " << std::hex << section->addr << " | " << section->data.size() << '\n'; 
    }
    std::cout << "------------------------------------------------------\n";

    for(auto [symbolName, symbol] : symbolTable) {
        for(auto [sectionName, section] : sectionTable) {
            if(section->reloc_table[symbolName].size() > 0) 
                std::cout << "reloc table for symbol " << symbolName << " in section " << sectionName << '\n';
                for(uint32 offset : section->reloc_table[symbolName]) {
                    std::cout << std::hex << offset << '\n';
                }
        }
    }

    // for(auto [sectionName, section] : sectionTable) {
    //     for(int i = 0; i < section->data.size(); i += 4) {
    //         uint32 binary_data = ((section->data[i]&0xff) << 24) | ((section->data[i+1]&0xff) << 16) | ((section->data[i+2]&0xff) << 8) | (section->data[i+3]&0xff);
    //         std::cout << std::hex << std::setw(2) << ((binary_data >> 24) & 0xff) << " "
    //                               << std::setw(2) << ((binary_data >> 16) & 0xff) << " "
    //                               << std::setw(2) << ((binary_data >> 8) & 0xff) << " "
    //                               << std::setw(2) << (binary_data & 0xff) << '\n';
    //     }
    // }
    std::cout << "------------------------------------------------------\n";
}