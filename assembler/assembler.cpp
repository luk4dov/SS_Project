#include "assembler.hpp"

extern int yyparse();
extern int yylineno;
extern FILE* yyin;

Assembler::~Assembler() {
    for(auto& [sectionName, section] : sectionTable) {
        delete section;
    }
    for(auto& [symbolName, symbol] : symbolTable) {
        delete symbol;
    }
}

void Assembler::removeLocalSymbols() {
    std::vector<std::string> localSymbols;
    for(const auto& [symbolName, symbol] : symbolTable) {                               // check for every symbol
        if(symbol->global) continue;                                                    // if symbol is global there is no need to do anything
        if(!symbol->defined) {                                                          // error, local symbol that's not defined
            throw LocalSymbolNotDefined(symbolName, std::string(inputFile));
        }
        
        localSymbols.push_back(symbolName);

        // symbol is local and defined, so we have to redirect relocation to symbol's section, and symbol's offset load to relocation address         

        for(const auto& [sectionName, section] : sectionTable) {
            for(uint32 offset : section->reloc_table[symbolName]) {                     // for every relocation of symbol in this section
                Symbol* sym = symbolTable[symbolName];

                section->reloc_table["." + sym->section].push_back(offset);             // create new relocation but over symbol's section
                section->data[offset] = symbol->offset & 0xff;                          // load symbol's offset from it's section to memory; little endian
                section->data[offset+1] = (symbol->offset >> 8) & 0xff;
                section->data[offset+2] = (symbol->offset >> 16) & 0xff;
                section->data[offset+3] = (symbol->offset >> 24) & 0xff;
            }
        }
    }

    for(const std::string& symbolName : localSymbols) { // remove entries in relocation tables for local symbols
        for(const auto& [sectionName, section] : sectionTable) {
            section->reloc_table.erase(symbolName);
        }
    }
    
    for(const std::string& symbolName : localSymbols) {
        delete symbolTable[symbolName];
        symbolTable.erase(symbolName);
    }
}

void Assembler::selectInstruction(std::string instr, int reg1, int reg2, uint32 immediate, std::string label, int type) {
    Instruction* instruction = Instruction::parsedInstructions[instr](instr, reg1, reg2, immediate, label, type);

    instruction->writeSectionData(sectionTable[section], symbolTable);

    delete instruction;
}

void Assembler::labelDefinition(std::string symbolName) {
    if (symbolTable.find(symbolName) == symbolTable.end()) { // symbol not in table, insert it
        symbolTable[symbolName] = new Symbol(section, sectionTable[section]->data.size(), false, true);
        return;
    }
    // check if the symbol is defined in table
    if(symbolTable[symbolName]->defined) { 
        throw SymbolRedefinition(symbolName, std::string(inputFile), yylineno);
    }
    symbolTable[symbolName]->section = section;
    symbolTable[symbolName]->offset = sectionTable[section]->data.size();
    symbolTable[symbolName]->defined = true;
}

void Assembler::selectDirective(std::string directive, std::string name, uint32 immediate) {
    if(directive == "global" || directive == "extern")  {
    
        if(symbolTable.find(name) == symbolTable.end()) {
            symbolTable[name] = new Symbol("UND", 0, true, false);
        }
        symbolTable[name]->global = true;

    } else if(directive == "section") {
        
        if(sectionTable.find(name) == sectionTable.end()) {
            sectionTable[name] = new Section(0, name);
            symbolTable["." + name] = new Symbol(name, 0, true, true);
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
                symbolTable[name] = new Symbol("UND", 0);   
            }
            sectionTable[section]->reloc_table[name].push_back(sectionTable[section]->data.size());
            sectionTable[section]->data.push_back(0x00);
            sectionTable[section]->data.push_back(0x00);
            sectionTable[section]->data.push_back(0x00);
            sectionTable[section]->data.push_back(0x00);
        }
    
    } else if (directive == "skip") {
        for(uint32 i = 0; i < immediate; ++i) {
            sectionTable[section]->data.push_back(0x00);
        }

    } else if (directive == "ascii") {
        // write every character from string into memory with terminate sign at the end
        for(char c : name) {
            sectionTable[section]->data.push_back(c);
        }
        sectionTable[section]->data.push_back('\0');
    
    }
}

void Assembler::printStat() {
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
            if(section->reloc_table[symbolName].size() > 0) {
                std::cout << "reloc table for symbol " << symbolName << " in section " << sectionName << '\n';
                for(uint32 offset : section->reloc_table[symbolName]) {
                    std::cout << std::hex << offset << '\n';
                }
            }
        }
    }
    std::cout << "------------------------------------------------------\n";
}

int Assembler::assemble() {
    yyin = fopen(inputFile, "r");

    try {
        int ret = yyparse();

        removeLocalSymbols();
        
        write();

        return ret;
    }
    catch (LiteralNotIn12Bits &e) { return -1; }
    catch (LocalSymbolNotDefined &e) { return -2; }
    catch (SymbolRedefinition &e) { return -3; }
}

void Assembler::write() {
    rw->write(std::string(outputFile), sectionTable, symbolTable);
}