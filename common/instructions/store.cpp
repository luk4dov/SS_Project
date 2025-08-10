#include "store.hpp"


StoreInstruction::StoreInstruction(short rs, short rd, uint32 immediate, std::string label, Section* section, std::unordered_map<std::string, Symbol*>& symbolTable, short type) :
            Instruction(), rs(rs), rd(rd), immediate(immediate), label(label), section(section), symbolTable(symbolTable), type(type) {};

Instruction* StoreInstruction::createInstruction(short rs, short rd, uint32 immediate, std::string label, Section* section, std::unordered_map<std::string, Symbol*>& symbolTable, short type) {
    return new StoreInstruction(rs, rd, immediate, label, section, symbolTable, type);
}

int StoreInstruction::write_section_data() {
    switch(type) {
        case 0: { // ST reg, $imm -> mem[imm] <= reg
            uint32 binary = serialize(ST, 2, 15, 0, rs, 0x4);
            write_binary(section, binary);
            binary = serialize(JMP, 0, 15, 0, 0, 0x4);
            write_binary(section, binary);
            write_binary(section, immediate);
            return 12;
        }
        case 1: { // ST reg, sym -> mem[sym] <= reg
            if(symbolTable.find(label) == symbolTable.end()) {
                symbolTable[label] = new Symbol(section->name, 0);
            }
            else if(symbolTable[label]->defined && section->data.size() - symbolTable[label]->offset < 0x7ff) { // can be pc relative
                uint32 binary = serialize(ST, 0x0, 15, 0, rs, symbolTable[label]->offset - section->data.size()); // mem[pc + o - offset] <= rs
                write_binary(section, binary);
                return 4;
            }
            // relocation must be done
            uint32 binary = serialize(ST, 0x2, 15, 0, rs, 0x4);
            write_binary(section, binary);
            binary = serialize(JMP, 0, 15, 0, 0, 4);
            write_binary(section, binary);
            section->reloc_table[label].push_back(section->data.size());
            write_binary(section, 0x0);
            return 12;
        }
        case 2: { // ST reg, [reg1] -> mem[reg] <= reg
            uint binary = serialize(ST, 0, rd, 0, rs, 0);
            write_binary(section, binary);
            return 4;
        }
        case 3: { // ST reg, [reg1+imm] -> mem[reg1+imm], reg
            if(immediate < 0xfff || immediate > 0x7ff) return -1;
            uint binary = serialize(ST, 0, rd, 0, rs, immediate);
            write_binary(section, binary);
            return 4;
        }
    }
    return 0;
}