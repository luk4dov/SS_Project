#include "call.hpp"

Instruction* CallInstruction::parsedInstruction(const std::string& instr, int r1, int r2, uint32 immediate, const std::string& label, int type) {
    return new CallInstruction(label, immediate, type);
}

Instruction* CallInstruction::binaryInstruction(int mod, int r1, int r2, int r3, int disp) {
    return new CallInstruction(mod, r1, r2, r3, disp);
}

int CallInstruction::writeSectionData(Section* section, std::unordered_map<std::string, Symbol*>& symbolTable) {
    // call instruction received immediate through immediate and identifier through the label, check which one is it
    uint32 binary;
    if(!label.empty()) { 
        Symbol* symbol = nullptr;
        if(symbolTable.find(label) == symbolTable.end()) { // label not found -> put symbol in symbol table and create relocation entry;
            symbolTable[label] = symbol = new Symbol(section->name, section->data.size());
        }
        if(!symbol && symbolTable[label]->defined && symbolTable[label]->section == section->name) { // already in symbol table, check if it is defined in the same section
            // check if offset is less than 12 signed bits
            if(section->data.size() - 0x7FF <= symbolTable[label]->offset) { // pc relative can be performed
                binary = serialize(CALL, 0, 15, 0, 0, section->data.size() - symbolTable[label]->offset);
                write_binary(section, binary);
                return 4;
            }
        }
        // relocation must be done, use jmp trick
        binary = serialize(CALL, 1, 15, 0, 0, 4);
        write_binary(section, binary);
        binary = serialize(JMP, 0, 15, 0, 0, 4);
        write_binary(section, binary);
        section->reloc_table[label].push_back(section->data.size());
        write_binary(section, 0x0); // fill the memory for relocation with zeros 
        return 12;
    }
    // if label is empty -> call immediate
    // relocation must be done against the current section (".section") so the linker can resolve to section->addr + offset
    binary = serialize(CALL, 1, 15, 0, 0, 4);
    write_binary(section, binary);
    binary = serialize(JMP, 0, 15, 0, 0, 4);
    write_binary(section, binary);
    section->reloc_table["." + section->name].push_back(section->data.size());
    write_binary(section, 0x0);
    return 12;
}