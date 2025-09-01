#include "store.hpp"


StoreInstruction::StoreInstruction(int r1, int r2, uint32 immediate, const std::string& label, int type, LoadStoreOC op) :
            Instruction("store"), r1(r1), r2(r2), immediate(immediate), label(label), type(type), op(op) {};

Instruction* StoreInstruction::createInstruction(const std::string& instr, int r1, int r2, uint32 immediate, const std::string& label, int type) {
    if(instr == "push") {
        return new StoreInstruction(r1, 14, 0, label, type, STACK);
    }
    else if (instr == "csrwr") {
        if(label == "status")
            r2 = STATUS;
        else if(label == "handler")
            r2 = HANDLER;
        else if(label == "cause")
            r2 = CAUSE;
        
        return new StoreInstruction(r1, r2, 0, label, type, CSR);
    }
    return new StoreInstruction(r1, r2, immediate, label, type, REGULAR);
}

int StoreInstruction::writeSectionData(Section* section, std::unordered_map<std::string, Symbol*>& symbolTable) {
    if(op == STACK) { // push
        uint32 binary = serialize(ST, 1, r2, 0, r1, -4); // ra, rb, rc, disp
        write_binary(section, binary);
        return 4;
    } else if(op == CSR) {
        uint32 binary = serialize(ST, 0, r2, 0, r1, 0); // mem[csr[r2]+r3+disp] <= r1
        write_binary(section, binary);
        return 4;
    }
    
    switch(type) {
        case 0: { // ST reg, $imm -> mem[imm] <= reg
            uint32 binary = serialize(ST, 2, 15, 0, r1, 0x4);
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
                uint32 binary = serialize(ST, 0x0, 15, 0, r1, symbolTable[label]->offset - section->data.size()); // mem[pc + o - offset] <= rs
                write_binary(section, binary);
                return 4;
            }
            // relocation must be done
            uint32 binary = serialize(ST, 0x2, 15, 0, r1, 0x4);
            write_binary(section, binary);
            binary = serialize(JMP, 0, 15, 0, 0, 4);
            write_binary(section, binary);
            section->reloc_table[label].push_back(section->data.size());
            write_binary(section, 0x0);
            return 12;
        }
        case 2: { // ST reg, [reg1] -> mem[reg] <= reg
            uint32 binary = serialize(ST, 0, r2, 0, r1, 0);
            write_binary(section, binary);
            return 4;
        }
        case 3: { // ST reg, [reg1+imm] -> mem[reg1+imm], reg
            if(immediate < 0xfff || immediate > 0x7ff) return -1;
            uint32 binary = serialize(ST, 0, r2, 0, r1, immediate);
            write_binary(section, binary);
            return 4;
        }
    }
    return 0;
}