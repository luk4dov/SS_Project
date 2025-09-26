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

                if(symbol->section != "ABS") {
                    section->reloc_table["." + sym->section].push_back(offset);
                }
                // section->reloc_table["." + sym->section].push_back(offset);             // create new relocation but over symbol's section
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

void Assembler::selectInstruction(const std::string& instr, int reg1, int reg2, uint32 immediate, const std::string& label, int type) {
    Instruction* instruction = Instruction::parsedInstructions[instr](instr, reg1, reg2, immediate, label, type);

    instruction->writeSectionData(sectionTable[section], symbolTable);

    delete instruction;
}

void Assembler::labelDefinition(const std::string& symbolName) {
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

void Assembler::selectDirective(const std::string& directive, const std::string& name, uint32 immediate) {
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

void Assembler::equDirective(uint32 type, const std::string& sym, const std::string& op1, const std::string& op2, uint32 num1, uint32 num2, uint32 operation) {
    if (sectionTable.find("ABS") == sectionTable.end()) {
        sectionTable["ABS"] = new Section(0, "ABS");
        symbolTable[".ABS"] = new Symbol("ABS", 0, true, true);
    }
    
    switch(type) {
        case 0: { // .equ sym, num1
            if(symbolTable.find(sym) != symbolTable.end() && symbolTable[sym]->defined) { // symbol already exists in literal pool
                throw SymbolRedefinition(sym, std::string(inputFile), yylineno);
            }
            
            if(symbolTable.find(sym) == symbolTable.end()) {
                symbolTable[sym] = new Symbol("ABS", num1, false, true);
            } else {
                symbolTable[sym]->defined = true;
                symbolTable[sym]->section = "ABS";
                symbolTable[sym]->offset = num1;
            }
            return;
        }
        case 1: { // .equ sym, num1 +- num2
            if(symbolTable.find(sym) != symbolTable.end() && symbolTable[sym]->defined) { // symbol already exists in literal pool
                throw SymbolRedefinition(sym, std::string(inputFile), yylineno);
            }

            uint32 value = num1 + (operation == 1 ? num2 : -num2);
            
            if(symbolTable.find(sym) == symbolTable.end()) {
                symbolTable[sym] = new Symbol("ABS", value, false, true);
            } else {
                symbolTable[sym]->defined = true;
                symbolTable[sym]->section = "ABS";
                symbolTable[sym]->offset = value;
            }
            return;
        }
        case 2: { // .equ sym, op1
            if(symbolTable.find(sym) != symbolTable.end() && (symbolTable[sym]->defined || symbolTable[sym]->section != "UND")) { // symbol already exists in symbol table
                throw SymbolRedefinition(sym, std::string(inputFile), yylineno);
            }

            // if operand1 is defined but not as .equ symbol, throw error
            if(symbolTable.find(op1) != symbolTable.end() && symbolTable[op1]->defined && symbolTable[op1]->section != "ABS") {
                throw InitializationWithNonEquSymbol(op1, std::string(inputFile), yylineno);
            }
            
            if(symbolTable.find(op1) != symbolTable.end() && symbolTable[op1]->defined) {
                
                uint32 value = symbolTable[op1]->offset;

                if(symbolTable.find(sym) == symbolTable.end()) {
                    symbolTable[sym] = new Symbol("ABS", value, false, true);
                } else {
                    symbolTable[sym]->defined = true;
                    symbolTable[sym]->section = "ABS";
                    symbolTable[sym]->offset = value;
                }
                return;
            } // op1 is not defined yet, add it to unresolved queue
            
            if(symbolTable.find(sym) == symbolTable.end()) { symbolTable[sym] = new Symbol("ABS", 0); }
            if(symbolTable.find(op1) == symbolTable.end()) { symbolTable[op1] = new Symbol("UND", 0); }
            unresolvedEqus.push(new EquExpression(2, sym, 0, op1, "", operation));
            return;
        }
        case 3: { // .equ sym, op1 +- num1
            if(symbolTable.find(sym) != symbolTable.end() && (symbolTable[sym]->defined || symbolTable[sym]->section != "UND")) { // symbol already exists in symbol table
                throw SymbolRedefinition(sym, std::string(inputFile), yylineno);
            }

            // if operand1 is defined but not as .equ symbol, throw error
            if(symbolTable.find(op1) != symbolTable.end() && symbolTable[op1]->defined && symbolTable[op1]->section != "ABS") {
                throw InitializationWithNonEquSymbol(op1, std::string(inputFile), yylineno);
            }
            
            if(symbolTable.find(op1) != symbolTable.end() && symbolTable[op1]->defined) {
                uint32 value = symbolTable[op1]->offset + (operation == 1 ? num1 : -num1);

                if(symbolTable.find(sym) == symbolTable.end()) {
                    symbolTable[sym] = new Symbol("ABS", value, false, true);
                } else {
                    symbolTable[sym]->defined = true;
                    symbolTable[sym]->section = "ABS";
                    symbolTable[sym]->offset = value;
                }
                return;
            }
            // operand not defined, add expression to unresolved queue
            if(symbolTable.find(sym) == symbolTable.end()) { symbolTable[sym] = new Symbol("ABS", 0); }
            if(symbolTable.find(op1) == symbolTable.end()) { symbolTable[op1] = new Symbol("UND", 0); }
            unresolvedEqus.push(new EquExpression(3, sym, num1, op1, "", operation));
            return;
        }
        case 4: { // .equ sym, op1 - op2
            if(symbolTable.find(sym) != symbolTable.end() && (symbolTable[sym]->defined || symbolTable[sym]->section != "UND")) { // symbol already exists in symbol table
                throw SymbolRedefinition(sym, std::string(inputFile), yylineno);
            }
            
            // both operands must be defined either as equ symbols, or as regular symbols in the same section
            if(symbolTable.find(op1) != symbolTable.end() && symbolTable[op1]->defined && 
               symbolTable.find(op2) != symbolTable.end() && symbolTable[op2]->defined &&
               symbolTable[op1]->section == symbolTable[op2]->section) { // both operands are defined and belong to the same section
                uint32 value = symbolTable[op1]->offset - symbolTable[op2]->offset;
                
                if(symbolTable.find(sym) == symbolTable.end()) {
                    symbolTable[sym] = new Symbol("ABS", value, false, true);
                } else {
                    symbolTable[sym]->defined = true;
                    symbolTable[sym]->section = "ABS";
                    symbolTable[sym]->offset = value;
                }
                return;
            }
            // at least one of the operands is not defined, add expression to unresolved queue
            if(symbolTable.find(sym) == symbolTable.end()) { symbolTable[sym] = new Symbol("ABS", 0); }
            if(symbolTable.find(op1) == symbolTable.end()) { symbolTable[op1] = new Symbol("UND", 0); }
            if(symbolTable.find(op2) == symbolTable.end()) { symbolTable[op2] = new Symbol("UND", 0); }
            unresolvedEqus.push(new EquExpression(4, sym, 0, op1, op2, operation));
        }
    }
}

void Assembler::resolveEqus() {
    EquExpression* expr;

    while(true) {
        int startSize = unresolvedEqus.size();
        if(startSize == 0) break; // all expressions resolved

        int currSize = unresolvedEqus.size();
        
        while(currSize-- > 0) {
            expr = unresolvedEqus.front();
            unresolvedEqus.pop();

            int ret = tryToResolve(expr);
            
            if(ret == 0) { // successfully resolved
                delete expr;
                continue;
            } else if(ret == -1) {
                throw InitializationWithNonEquSymbol(expr->operand1, std::string(inputFile), yylineno);
            } else if(ret == 1) { // operand not defined yet
                unresolvedEqus.push(expr);
                continue;
            }
        }

        currSize = unresolvedEqus.size();

        if(startSize == currSize) {
            throw CircularDependencyInEqu(std::string(inputFile));
        }
    }
}

int Assembler::tryToResolve(EquExpression* expr) {
    switch(expr->type) {
        case 2: { // .equ sym, op1
            // if operand1 is defined but not as .equ symbol, error
            if(symbolTable[expr->operand1] -> defined && symbolTable[expr->operand1]->section != "ABS") {
                return -1;
            }

            if(symbolTable[expr->operand1]->defined) {
                uint32 value = symbolTable[expr->operand1]->offset;

                symbolTable[expr->symbol]->defined = true;
                symbolTable[expr->symbol]->section = "ABS";
                symbolTable[expr->symbol]->offset = value;
                return 0;
            }
            // operand not defined yet
            return 1; // flag to try resolving expression again later
        }
        case 3: { // .equ sym, op1 +- num1
            if(symbolTable[expr->operand1] -> defined && symbolTable[expr->operand1]->section != "ABS") {
                return -1;
            }

            if(symbolTable[expr->operand1]->defined) {
                uint32 value = symbolTable[expr->operand1]->offset + (expr->operation == 1 ? expr->value : -expr->value);
                
                symbolTable[expr->symbol]->defined = true;
                symbolTable[expr->symbol]->section = "ABS";
                symbolTable[expr->symbol]->offset = value;
                return 0;
            }
            return 1; // flag to try again later
        }
        case 4: { // .equ sym, op1 - op2
            // check if op1 and op2 are defined in the same section
            // symbols in ABS section
            if(symbolTable[expr->operand1] -> section == "ABS" && symbolTable[expr->operand2]->section == "ABS") {
                if(symbolTable[expr->operand1] -> defined && symbolTable[expr->operand2]->defined) {
                    uint32 value = symbolTable[expr->operand1]->offset - symbolTable[expr->operand2]->offset;
                    
                    symbolTable[expr->symbol]->defined = true;
                    symbolTable[expr->symbol]->section = "ABS";
                    symbolTable[expr->symbol]->offset = value;
                    return 0;
                }
                return 1;
            }
            // symbols in non ABS sections, must be defined in the same section
            if(symbolTable[expr->operand1] -> defined && symbolTable[expr->operand2]->defined && 
               symbolTable[expr->operand1]->section == symbolTable[expr->operand2]->section) {
                uint32 value = symbolTable[expr->operand1]->offset - symbolTable[expr->operand2]->offset;
                
                symbolTable[expr->symbol]->defined = true;
                symbolTable[expr->symbol]->section = "ABS";
                symbolTable[expr->symbol]->offset = value;
                return 0;
            }
            // otherwise, error 
            return -1;
        }
    }
    
    return 0;
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

    // for(auto [symbolName, symbol] : symbolTable) {
    //     for(auto [sectionName, section] : sectionTable) {
    //         if(section->reloc_table[symbolName].size() > 0) {
    //             std::cout << "reloc table for symbol " << symbolName << " in section " << sectionName << '\n';
    //             for(uint32 offset : section->reloc_table[symbolName]) {
    //                 std::cout << std::hex << offset << '\n';
    //             }
    //         }
    //     }
    // }
    // std::cout << "------------------------------------------------------\n";
}

int Assembler::assemble() {
    yyin = fopen(inputFile, "r");

    try {
        int ret = yyparse();

        resolveEqus();

        removeLocalSymbols();

        write();

        return ret;
    }
    catch (LiteralNotIn12Bits &e) { return -1; }
    catch (LocalSymbolNotDefined &e) { return -2; }
    catch (SymbolRedefinition &e) { return -3; }
    catch (InitializationWithNonEquSymbol &e) { return -4; }
    catch (CircularDependencyInEqu &e) { return -5; }
}

void Assembler::write() {
    rw->write(std::string(outputFile), sectionTable, symbolTable);
}