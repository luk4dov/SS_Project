#include "load.hpp"


LoadInstruction::LoadInstruction(short rd, short rs, uint32 immediate, std::string label, Section* section, std::unordered_map<std::string, Symbol*>& symbolTable, short type) :
            Instruction(), rd(rd), rs(rs), immediate(immediate), label(label), section(section), symbolTable(symbolTable), type(type) {};

Instruction* LoadInstruction::createInstruction(short rd, short rs, uint32 immediate, std::string label, Section* section, std::unordered_map<std::string, Symbol*>& symbolTable, short type) {
    return new LoadInstruction(rd, rs, immediate, label, section, symbolTable, type);
}

int LoadInstruction::write_section_data() {
    switch(type) {
        case 0: { // LD $imm, reg -> reg <= imm -> jmp trick                                
            uint32 binary = serialize(LD, 0x2, rd, 15, 0, 4); // -> load rd, [pc+0+4]       //  i :  load rd, [pc+r0+4]
            write_binary(section, binary);                                                  //  i+4: jmp pc+4
            binary = serialize(JMP, 0, 15, 0, 0, 4);                                        //  i+8  immediate
            write_binary(section, binary);
            write_binary(section, immediate);
            return 12;
        }
        case 1: { // LD $sym, reg -> reg <= sym                                             // after relocation:
            uint32 binary = serialize(LD, 0x2, rd, 15, 0, 4); // -> load rd, [pc+0+4]       //  i :  load rd, [pc+r0+4]
            write_binary(section, binary);                                                  //  i+4: jmp pc+4
            binary = serialize(JMP, 0, 15, 0, 0, 4);                                        //  i+8  a
            write_binary(section, binary);
            section->reloc_table[label].push_back(section->data.size());
            write_binary(section, immediate);
            return 12; 
        }
        case 2: { // LD imm, reg -> reg <= mem[imm]
            if(immediate < 0x7ff) {
                uint32 binary = serialize(LD, 0x2, rd, 0, 0, immediate);
                write_binary(section, binary);
                return 4;
            } else {
                uint32 binary = serialize(LD, 0x2, rd, 15, 0, 4);
                write_binary(section, binary);
                binary = serialize(JMP, 0, 15, 0, 0, 4);
                write_binary(section, binary);
                write_binary(section, immediate);
                binary = serialize(LD, 0x2, rd, rd, 0, 0);
                write_binary(section, binary);
                return 16;
            }
        }
        case 3: { // LD sym, reg -> reg <= mem[sym]
            Symbol* symbol = nullptr;
            if(symbolTable.find(label) == symbolTable.end()) {
                symbol = new Symbol(label, 0);
            }
            if(symbol) { // if symbol didn't exist in symbol table, insert it; add relocation
                symbolTable[label] = symbol;
            }
            else if(symbolTable[label]->defined && section->data.size() - symbolTable[label]->offset < 0x7ff) { // can be pc relative
                uint32 binary = serialize(LD, 0x2, rd, 15, 0, symbolTable[label]->offset - section->data.size()); // ra <= mem[rb+rc+disp] = reg <= mem[pc + 0 + (-offset)];
                return 4;
            }
            uint32 binary = serialize(LD, 0x2, rd, 15, 0, 4);
            write_binary(section, binary);
            binary = serialize(JMP, 0, 15, 0, 0, 4);
            write_binary(section, binary);
            section->reloc_table[label].push_back(section->data.size());
            write_binary(section, 0x0);
            binary = serialize(LD, 0x2, rd, rd, 0, 0);
            write_binary(section, binary);
            return 16;
        }
        case 4: { // LD %reg, reg -> reg <= %reg
            uint32 binary = serialize(LD, 0x1, rd, rs, 0, 0);
            write_binary(section, binary);
            return 4;
        }
        case 5: { // LD [%reg], reg -> mem[%reg]
            uint32 binary = serialize(LD, 0x2, rd, rs, 0, 0);
        }
        case 6: { // LD [%reg+imm], reg -> mem[%reg + imm]
            if(immediate > 0x7FF || immediate < 0xFFF) return -1;
            
            uint32 binary = serialize(LD, 0x2, rd, rs, 0, immediate);
            write_binary(section, binary);
            return 4;
        }
    }
    return 0;
}