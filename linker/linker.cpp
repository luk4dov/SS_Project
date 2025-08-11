#include "../common/types.hpp"
#include "../common/binaryrw.hpp"
#include <iomanip>

static std::unordered_map<std::string, Section*> sectionTable;
static std::unordered_map<std::string, Symbol*> symbolTable;

void printStat() {
    std::cout << "Symbol table: symbol  |  section  |  defined  |  bind \n";
    for(auto [symbolName, symbol] : symbolTable) {
        std::cout << symbolName << " | " << symbol->section << " | " << symbol->defined << " | " << symbol->global << '\n';
    }
    std::cout << "------------------------------------------------------\n";
    std::cout << "Section table: name  |  offset  |  size \n";
    for(auto [sectionName, section] : sectionTable) {
        std::cout << sectionName << " | " << 0 << " | " << section->data.size() << '\n'; 
    }
    std::cout << "------------------------------------------------------\n";

    for(auto [symbolName, symbol] : symbolTable) {
        for(auto [sectionName, section] : sectionTable) {
            if(section->reloc_table.size() > 0) 
                std::cout << "reloc table for symbol " << symbolName << " in section " << sectionName << '\n';
            for(uint32 offset : section->reloc_table[symbolName]) {
                std::cout << offset << '\n';
            }
        }
    }

    for(auto [sectionName, section] : sectionTable) {
        std::cout << "reloc table for section " << sectionName << " in section " << sectionName << '\n';
        for(uint32 offset : section->reloc_table[sectionName]) {
            std::cout << offset << '\n';
        }
    }
}

int main(int argc, char* argv[]) {
    std::vector<std::string> inputFiles = {};
    bool relocatable = false;
    bool hex = false;
    std::string outputFile = "izlazni_fajl_linker.o"; 
    // -place option data
    std::unordered_map<std::string, uint32_t> sectionAddress = {};
    uint32_t maxSectionAddress = 0;
    std::string maxSectionName = "";

    for(int i = 1; i < argc; ++i) {
        if(std::string(argv[i]) == "-relocatable") { 
            if(hex)
                return -1;
            relocatable = true; 
        }
        else if(std::string(argv[i]) == "-hex") { 
            if(relocatable) 
                return -1; 
            hex = true;
        } 
        else if(std::string(argv[i]) == "-o") {
            if(i+1 == argc) return -2;
            else outputFile = std::string(argv[++i]); 
        }
        else if(std::string(argv[i]).at(0) == '-') {
            if(relocatable) { 
                return -1; 
            }
            std::string option = std::string(argv[i]);
            std::string prefix = "-place=";
            option.erase(0, prefix.size());

            size_t atPos = option.find("@");
            if(atPos == std::string::npos)  {
                std::cerr << "Invalid format\n";
                return -3;
            }

            std::string section = option.substr(0, atPos);
            std::string addr = option.substr(atPos+1);

            uint32 address = std::stoul(addr, nullptr, 0);
            if(sectionAddress.find(section) != sectionAddress.end()) {
                std::cerr << "Error: section " << section << " is already placed\n";
                return -5;
            }
            sectionAddress[section] = address;
            if(address > maxSectionAddress) {
                maxSectionAddress = address;
                maxSectionName = section;
            }
        } else { // input files
            inputFiles.push_back(std::string(argv[i]));
        }
    }
    if(inputFiles.empty()) {
        std::cerr << "No input files specified\n";
        return -2;
    }

    sectionTable = {};
    symbolTable = {};

    std::unordered_map<std::string, Section*> tempSectionTable = {};
    std::unordered_map<std::string, Symbol*> tempSymbolTable = {};

    BinaryRW* rw = new BinaryRW(tempSectionTable, tempSymbolTable);
    for(std::string file : inputFiles) {
        rw->read(file);

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
            if(symbolTable[tempSymbolName]->defined) {
                if(tempSymbol->defined) {  // two global symbols with the same name and both defined, error
                    std::cerr << "Error: symbol " << tempSymbolName << " is defined in multiple files\n"; return -4;
                }
            } else if(tempSymbol->defined) { // we found definition of symbol, update symbol table
                symbolTable[tempSymbolName] = tempSymbol;
                symbolTable[tempSymbolName]->offset += sectionTable[tempSymbol->section]->data.size() - tempSectionTable[tempSymbol->section]->data.size(); // adjust offset to the new section
            }
        }

        tempSectionTable.clear();
        tempSymbolTable.clear();
    }

    if(!hex && !relocatable) {
        std::cerr << "Error: Must use -relocatable or -hex option\n";
        return -1;
    }

    // if relocatable, just write to binary file
    if(relocatable) {
        delete rw;
        rw = new BinaryRW(sectionTable, symbolTable);
        rw->write(outputFile);
        delete rw;
    } else if(hex) {

        // it's hex, create executable file
        // before creating executable, place sections at their addresses if there was -place option, and check if some sections are going to overlap
        // conditions: all relocations must be resolved, every symbol in symbol table must be defined and there can be no duplicate symbols
        
        // check if all symbols are defined
        for(auto& [symbolName, symbol] : symbolTable) {
            if(!symbol->defined) {
                std::cerr << "Error: symbol " << symbolName << " is not defined\n";
                return -4;
            }
        }

        for(auto& [sectionName, section] : sectionTable) {
            section->addr = maxSectionAddress + sectionTable[maxSectionName]->data.size();
        }
        for(auto& [sectionName, startingAddress] : sectionAddress) {
            if(sectionTable.find(sectionName) == sectionTable.end()) {
                std::cerr << "Error: section " << sectionName << " not found in section table\n";
                return -6;
            }
            sectionTable[sectionName]->addr = startingAddress;
            // go through all sections and check if they overlap

            for(auto& [otherSectionName, otherSection] : sectionTable) {
                if(otherSectionName == sectionName) continue; // skip self

                // check if sections overlap
                if((sectionTable[sectionName]->addr >= otherSection->addr && sectionTable[sectionName]->addr < otherSection->addr + otherSection->data.size()) ||
                   (otherSection->addr >= sectionTable[sectionName]->addr && otherSection->addr < sectionTable[sectionName]->addr + sectionTable[sectionName]->data.size())) {
                    std::cerr << "Error: sections " << sectionName << " and " << otherSectionName << " overlap\n";
                    return -7;
                }
            }
        }
        // after checking overlaps, it's time to place other sections behind the one with the biggest address
        uint32_t currentAddress = maxSectionAddress + sectionTable[maxSectionName]->data.size();
        for(auto& [sectionName, section] : sectionTable) {
            if(sectionAddress.find(sectionName) != sectionAddress.end()) {
                continue;
            }
            section->addr = currentAddress;
            currentAddress += section->data.size();
        }

        // resolve relocations
        for(auto& [sectionName, section] : sectionTable) {
            for(auto& [symbolName, offsets] : section->reloc_table) {
                Symbol* symbol = symbolTable[symbolName];
                uint32_t symbolAddress = sectionTable[symbol->section]->addr + symbol->offset;

                for(uint32 offset : offsets) {
                    section->data[offset] += (symbolAddress >> 24) & 0xff;
                    section->data[offset+1] += (symbolAddress >> 16) & 0xff;
                    section->data[offset+2] += (symbolAddress >> 8) & 0xff;
                    section->data[offset+3] += symbolAddress & 0xff;
                }
            }
        }

        // write to binary file
        delete rw;
        rw = new BinaryRW(sectionTable, symbolTable);
        rw->write(outputFile);
        delete rw;
    }

    printStat();

    for(auto [sectionName, section] : sectionTable) {
        for(int i = 0; i < section->data.size(); i += 4) {
            uint32 binary_data = ((section->data[i]&0xff) << 24) | ((section->data[i+1]&0xff) << 16) | ((section->data[i+2]&0xff) << 8) | (section->data[i+3]&0xff);
            std::cout << std::hex << std::setw(2) << ((binary_data >> 24) & 0xff) << " "
                                  << std::setw(2) << ((binary_data >> 16) & 0xff) << " "
                                  << std::setw(2) << ((binary_data >> 8) & 0xff) << " "
                                  << std::setw(2) << (binary_data & 0xff) << '\n';
        }
    }

    return 0;
}

