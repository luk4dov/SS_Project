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
    std::string outputFile = ""; 
    std::unordered_map<std::string, uint32> sectionAddress = {};

    for(int i = 1; i < argc; ++i) {
        if(std::string(argv[i]) == "-relocatable") { relocatable = true; if(hex) return -1; }
        else if(std::string(argv[i]) == "-hex") { hex = true; if(relocatable) return -1;} 
        else if(std::string(argv[i]) == "-o") {
            if(i+1 == argc) return -2;
            else outputFile = std::string(argv[++i]); 
        }
        else if(std::string(argv[i]).at(0) == '-') {
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
            sectionAddress[section] = address;
        } else { // input files
            inputFiles.push_back(std::string(argv[i]));
        }
    }
    // if(!hex && !relocatable) return -3;
    
    sectionTable = {};
    symbolTable = {};

    BinaryRW* rw = new BinaryRW(sectionTable, symbolTable);
    for(std::string file : inputFiles) {
        rw->read(file);
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

