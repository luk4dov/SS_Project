#include "load.hpp"

LoadInstruction::LoadInstruction(int r1, int r2, uint32 immediate, const std::string& label, int type, LoadStoreOC op) :
            Instruction("load"), r1(r1), r2(r2), immediate(immediate), label(label), type(type), op(op) {};

LoadInstruction::LoadInstruction(int mod, int r1, int r2, int r3, int disp) : Instruction("load"), r1(r1), r2(r2), r3(r3), disp(disp) {
    if(mod < 3) op = REGULAR;
    else if (mod == 4) op = STACK;
    else op = CSR;    
};

Instruction* LoadInstruction::parsedInstruction(const std::string& instr, int r1, int r2, uint32 immediate, const std::string& label, int type) {
    LoadStoreOC op = REGULAR;
    if(instr == "ret") {
        return new LoadInstruction(15, 14, 0, "", 0, STACK);
    }
    else if(instr == "pop") {
        return new LoadInstruction(r1, 14, 0, "", 0, STACK);
    } else if(instr == "csrrd" || instr == "csrwr") {
        if(label == "status")
            r2 = STATUS;
        else if(label == "handler")
            r2 = HANDLER;
        else if(label == "cause")
            r2 = CAUSE;

        return new LoadInstruction(r1, r2, 0, instr, 0, CSR);
    }
    return new LoadInstruction(r1, r2, immediate, label, type, op);
}

Instruction* LoadInstruction::binaryInstruction(int mod, int r1, int r2, int r3, int disp) {
    return new LoadInstruction(mod, r1, r2, r3, disp);
}

int LoadInstruction::writeSectionData(Section* section, std::unordered_map<std::string, Symbol*>& symbolTable) {
    if(op == STACK) {
        uint32 binary = serialize(LOAD, 3, r1, r2, 0, 4); // pop r1 { r1 <= mem[sp]; sp = sp + 4; }
        write_binary(section, binary);
        return 4;
    } else if(op == CSR) {
        uint32 binary;
        if(label == "csrwr") {
            binary = serialize(LOAD, 4, r1, r2, 0, 0); // csr[r2] <= r1
        } else if(label == "csrrd") {
            binary = serialize(LOAD, 0, r1, r2, 0, 0); // r1 <= csr[r2]
        }
        write_binary(section, binary);
        return 4;
    }
    
    switch(type) {
        case 0: { // LOAD $imm, reg -> reg <= imm -> jmp trick
            if(immediate > MIN_VAL && immediate < MAX_VAL) {
                uint32 binary = serialize(LOAD, 1, r1, 0, 0, immediate); // ra <= mem[rb+rc+disp] = reg <= mem[r0 + 0 + imm];
                write_binary(section, binary);
                return 4;
            }
            uint32 binary = serialize(LOAD, 2, r1, 15, 0, 4); // -> load rd, [pc+0+4]       //  i :  load rd, [pc+r0+4]
            write_binary(section, binary);                                                  //  i+4: jmp pc+4
            binary = serialize(JMP, 0, 15, 0, 0, 4);                                        //  i+8  immediate
            write_binary(section, binary);
            write_binary(section, immediate);
            return 12;
        }
        case 1: { // LD $sym, reg -> reg <= sym   
            if(symbolTable.find(label) == symbolTable.end()) {
                symbolTable[label] = new Symbol(section->name, 0);
            }
            else if(symbolTable[label]->defined && section->data.size() - symbolTable[label]->offset < 0x7ff) { // can be pc relative
                uint32 binary = serialize(LOAD, 2, r1, 15, 0, symbolTable[label]->offset - section->data.size()); // ra <= mem[rb+rc+disp] = reg <= mem[pc + 0 + (-offset)];
                write_binary(section, binary);
                return 4;
            }                                                                               // after relocation:
            uint32 binary = serialize(LOAD, 2, r1, 15, 0, 4); // -> load rd, [pc+0+4]       //  i :  load rd, [pc+r0+4]
            write_binary(section, binary);                                                  //  i+4: jmp pc+4
            binary = serialize(JMP, 0, 15, 0, 0, 4);                                        //  i+8  a
            write_binary(section, binary);
            section->reloc_table[label].push_back(section->data.size());
            write_binary(section, 0x0);
            return 12; 
        }
        case 2: { // LD imm, reg -> reg <= mem[imm]
            if(immediate < MAX_VAL) {
                uint32 binary = serialize(LOAD, 2, r1, 0, 0, immediate);
                write_binary(section, binary);
                return 4;
            } else {
                uint32 binary = serialize(LOAD, 2, r1, 15, 0, 4);
                write_binary(section, binary);
                binary = serialize(JMP, 0, 15, 0, 0, 4);
                write_binary(section, binary);
                write_binary(section, immediate);
                binary = serialize(LOAD, 2, r1, r1, 0, 0);
                write_binary(section, binary);
                return 16;
            }
        }
        case 3: { // LD sym, reg -> reg <= mem[sym]
            if(symbolTable.find(label) == symbolTable.end()) {
                symbolTable[label] = new Symbol(section->name, 0);
            }
            else if(symbolTable[label]->defined && section->data.size() - symbolTable[label]->offset < 0x7ff) { // can be pc relative
                uint32 binary = serialize(LOAD, 0x2, r1, 15, 0, symbolTable[label]->offset - section->data.size()); // ra <= mem[rb+rc+disp] = reg <= mem[pc + 0 + (-offset)];
                write_binary(section, binary);
                return 4;
            }
            uint32 binary = serialize(LOAD, 0x2, r1, 15, 0, 4);
            write_binary(section, binary);
            binary = serialize(JMP, 0, 15, 0, 0, 4);
            write_binary(section, binary);
            section->reloc_table[label].push_back(section->data.size());
            write_binary(section, 0x0);
            binary = serialize(LOAD, 0x2, r1, r1, 0, 0);
            write_binary(section, binary);
            return 16;
        }
        case 4: { // LD %reg, reg -> reg <= %reg
            uint32 binary = serialize(LOAD, 0x1, r1, r2, 0, 0);
            write_binary(section, binary);
            return 4;
        }
        case 5: { // LD [%reg], reg -> mem[%reg]
            uint32 binary = serialize(LOAD, 0x2, r1, r2, 0, 0);
            write_binary(section, binary);
            return 4;
        }
        case 6: { // LD [%reg+imm], reg -> mem[%reg + imm]
            if(immediate > MAX_VAL || immediate < MIN_VAL) return -1;
            
            uint32 binary = serialize(LOAD, 0x2, r1, r2, 0, immediate);
            write_binary(section, binary);
            return 4;
        }
    }
    return 0;
}