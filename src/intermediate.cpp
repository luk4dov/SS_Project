#include <iostream>
#include <string.h>

#include "../common/types.hpp"
#include "../common/instruction.hpp"

extern int yyparse();

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
        if(!symbol->defined) return; // error, local symbol that's not defined
        // symbol is local and defined, so we have to redirect relocation to symbol's section, and symbol's offset load to relocation address 
        localSymbols.push_back(symbolName);

        for(auto& [sectionName, section] : sectionTable) {
            for(uint32 offset : section->reloc_table[symbolName]) { // for every relocation of symbol in this section
                Symbol* sym = symbolTable[symbolName];

                section->reloc_table[sym->section].push_back(offset); // create new relocation but over symbol's section
                section->data[offset] = symbol->offset & 0xff; // load symbol's offset from it's section to memory; little endian
                section->data[offset+1] = (symbol->offset >> 8) & 0xff;
                section->data[offset+2] = (symbol->offset >> 16) & 0xff;
                section->data[offset+3] = (symbol->offset >> 24) & 0xff;
            }
        }
        delete symbolTable[symbolName];
    }
    for(std::string symbol : localSymbols) {
        symbolTable.erase(symbol);
    }
}


void selectInstructionWrap(std::string name, short reg1, short reg2, uint32 immediate, std::string label, Section* section, std::unordered_map<std::string, Symbol*>& symbolTable, short type) {
    Instruction* instruction = Instruction::instructions[name](reg1, reg2, immediate, label, section, symbolTable, type);
    
    if (instruction) int ret = instruction->write_section_data();
    
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
    symbolTable[name]->section = section;
    symbolTable[name]->offset = sectionTable[section]->data.size();
    symbolTable[name]->defined = true;
}

void selectDirective(const char* dir, const char* nm, uint32 immediate) {
    std::string directive = std::string(dir);
    std::string name = std::string(nm);

    if(directive == "global" || directive == "extern")  {
        // if it doesn't exist in symbolTable, insert it, anyways check the global flag
        if(symbolTable.find(name) == symbolTable.end()) {
            symbolTable[name] = new Symbol("UND", 0, true);
        }
        symbolTable[name]->global = true;

    } else if(directive == "section") {
        // check if section exists in sectionTable, otherwise create new section
        if(sectionTable.find(name) == sectionTable.end()) {
            sectionTable[name] = new Section(0, 0, name);
        }
        section = name; // change current section name
    
    } else if(directive == "word") {
        if(name.empty()) { // .word immediate
            sectionTable[section]->data.push_back(immediate & 0xff);
            sectionTable[section]->data.push_back((immediate >> 8) & 0xff);
            sectionTable[section]->data.push_back((immediate >> 16) & 0xff);
            sectionTable[section]->data.push_back((immediate >> 24) & 0xff);
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

int main(int argc, char* argv[]) {
    sectionTable = {};
    symbolTable = {};
    section="";

    std::cout << "Parsing started\n";

    int ret = yyparse();

    std::cout << "\nParsing finished\n";
    return 0;
}