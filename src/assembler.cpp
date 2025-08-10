#include <iostream>
#include <string.h>

#include "../common/types.hpp"
#include "../common/instruction.hpp"
#include "../common/binaryrw.hpp"

extern int yyparse();
extern int yylineno;
extern FILE* yyin;

static std::string section;
static std::unordered_map<std::string, Section*> sectionTable;
static std::unordered_map<std::string, Symbol*> symbolTable;

// for every local symbol, we must exclude it from symbolTable, but before that every
// relocation for that symbol must be overwritten to be done from the section where that symbol is defined
// while addend (offset from symbol's section) will be written into memory where that symbol must be resolved

// linker's job will be, after joining sections from multiple files, to add symbol's section address to the address of relocation
void removeLocalSymbols() {
    std::vector<std::string> localSymbols;

    for(auto& [symbolName, symbol] : symbolTable) { // check for every symbol
        if(symbol->global) continue; // if symbol is global there is no need to do anything
        if(!symbol->defined) { localSymbols.push_back(symbolName); std::cout << symbolName << " not defined!\n"; continue; } // error, local symbol that's not defined
        // symbol is local and defined, so we have to redirect relocation to symbol's section, and symbol's offset load to relocation address 
        
        localSymbols.push_back(symbolName);

        for(auto& [sectionName, section] : sectionTable) {
            for(uint32 offset : section->reloc_table[symbolName]) { // for every relocation of symbol in this section
                Symbol* sym = symbolTable[symbolName];

                section->reloc_table[sym->section].push_back(offset); // create new relocation but over symbol's section
                section->data[offset] = (symbol->offset >> 24) & 0xff; // load symbol's offset from it's section to memory; little endian
                section->data[offset+1] = (symbol->offset >> 16) & 0xff;
                section->data[offset+2] = (symbol->offset >> 8) & 0xff;
                section->data[offset+3] = symbol->offset & 0xff;
            }
        }
    }
    for(std::string symbol : localSymbols) {
        delete symbolTable[symbol];
        symbolTable.erase(symbol);
    }
}


void selectInstructionWrap(std::string name, short reg1, short reg2, uint32 immediate, std::string label, Section* section, std::unordered_map<std::string, Symbol*>& symbolTable, short type) {
    Instruction* instruction = Instruction::instructions[name](reg1, reg2, immediate, label, section, symbolTable, type);

    int ret = instruction->write_section_data();
    
    if (instruction) delete instruction;
}
void selectInstruction(const char* name, short reg1, short reg2, uint32 immediate, const char* label, short type) {
    selectInstructionWrap(std::string(name), reg1, reg2, immediate, std::string(label), sectionTable[section], symbolTable, type);
}

void labelDefinition(const char* name) {
    std::string symbol = std::string(name);
    if (symbolTable.find(symbol) == symbolTable.end()) { // symbol not in table, insert it
        
        symbolTable[symbol] = new Symbol(section, sectionTable[section]->data.size(), false, true);
        return;
    }
    // check if the symbol is defined in table
    if(symbolTable[symbol]->defined) { std::cout << "redefinition of symbol:" << symbol << '\n'; return; }
    symbolTable[symbol]->section = section;
    symbolTable[symbol]->offset = sectionTable[section]->data.size();
    symbolTable[symbol]->defined = true;
}

void selectDirective(const char* dir, const char* nm, uint32 immediate) {
    std::string directive = std::string(dir);
    std::string name = std::string(nm);

    if(directive == "global" || directive == "extern")  {
        // if it doesn't exist in symbolTable, insert it, anyways check the global flag
        if(symbolTable.find(name) == symbolTable.end()) {
            symbolTable[name] = new Symbol("UND", 0, true, false);
        }
        symbolTable[name]->global = true;

    } else if(directive == "section") {
        // check if section exists in sectionTable, otherwise create new section
        if(sectionTable.find(name) == sectionTable.end()) {
            sectionTable[name] = new Section(0, 0, name);
            // symbolTable[name] = new Symbol(section, 0, true, true);
        }
        section = name; // change current section name

    } else if(directive == "word") {
        if(name.empty()) { // .word immediate
            sectionTable[section]->data.push_back((immediate >> 24) & 0xff);
            sectionTable[section]->data.push_back((immediate >> 16) & 0xff);
            sectionTable[section]->data.push_back((immediate >> 8) & 0xff);
            sectionTable[section]->data.push_back(immediate & 0xff);
        } else { // do reloc, it's easier
            // check if the symbol exists in symbolTable
            if(symbolTable.find(name) == symbolTable.end()) {
                symbolTable[name] = new Symbol("UND", 0, false, false);   
            }
            sectionTable[section]->reloc_table[name].push_back(sectionTable[section]->data.size());
            sectionTable[section]->data.push_back(0x0);
            sectionTable[section]->data.push_back(0x0);
            sectionTable[section]->data.push_back(0x0);
            sectionTable[section]->data.push_back(0x0);
        }
    
    } else if (directive == "skip") {
        for(int i = 0; i < immediate; ++i) { // 1 push_back(0) -> 00000000b
            sectionTable[section]->data.push_back(0);
        }

    } else if (directive == "ascii") {
        // write every character from string into memory with terminate sign at the end
        for(char c : name) {
            sectionTable[section]->data.push_back(c);
        }
        sectionTable[section]->data.push_back('\0');

    } else if (directive == "end") {
        removeLocalSymbols();
    }
}

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
    if(argc < 2 || argc > 4 || argc == 3) return -1;
    
    char* inputFile = argv[argc-1];
    char* outputFile;
    if(argc > 2) 
        outputFile = argv[2];
    else
        outputFile = "./izlazni_fajl.o";

    sectionTable = {};
    symbolTable = {};
    section="";
    
    yyin = fopen(inputFile, "r");
    int ret = yyparse();

    printStat();

    BinaryRW* rw = new BinaryRW(sectionTable, symbolTable);
    rw->write(std::string(outputFile));

    return 0;
}


