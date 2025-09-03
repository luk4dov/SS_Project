#include "jmp.hpp"

JmpInstruction::JmpInstruction(int r1, int r2, uint32 immediate, const std::string& label, int type, JMPCondition cond) :
    Instruction("jmp"), r1(r1), r2(r2), immediate(immediate), label(label), type(type), cond(cond)  {}

JmpInstruction::JmpInstruction(int mod, int r1, int r2, int r3, int disp) :
    Instruction("jmp"), r1(r1), r2(r2), r3(r3), disp(disp), cond(JMPCondition(mod % 8)), mod(mod) {}

Instruction* JmpInstruction::parsedInstruction(const std::string& instr, int reg1, int reg2, uint32 immediate, const std::string& label, int type) {
    JMPCondition cond = ALWAYS;

    if(instr == "bne") {
        cond = BNE;
    } else if(instr == "beq") {
        cond = BEQ;
    } else if(instr == "bgt") {
        cond = BGT;
    }

    return new JmpInstruction(reg1, reg2, immediate, label, type, cond);
}

Instruction* JmpInstruction::binaryInstruction(int mod, int r1, int r2, int r3, int disp) {
    return new JmpInstruction(mod, r1, r2, r3, disp);
}

int JmpInstruction::writeSectionData(Section* section, std::unordered_map<std::string, Symbol*>& symbolTable) {
    uint32 binary = 0;
    if(type == 0) { // jmp imm;
        if (immediate <= 0x7ff) {
            uint32 binary = serialize(JMP, cond, 0, r1, r2, immediate);
            write_binary(section, binary);
            return 4;
        } else { // jmp trick
            binary = serialize(JMP, 0x8+cond, 15, r1, r2, 0x4);             // memory:
            write_binary(section, binary);                                  //    i  : jmp [pc+4]
            binary = serialize(JMP, 0, 15, 0, 0, 0x4);                 //   i+4 : jmp 4
            write_binary(section, binary);                                  //   i+8 : immediate
            write_binary(section, immediate);                               //   i+12: next instruction...
            return 12;
        }
    }
    // jmp symbol
    Symbol* symbol = nullptr;
    if(symbolTable.find(label) == symbolTable.end()) {
        symbolTable[label] = symbol = new Symbol(section->name, section->data.size());
    }
    if(!symbol && symbolTable[label]->defined && symbolTable[label]->section == section->name) { // already in symbol table, check if it is defined in the same section
            // check if offset is less than 12 signed bits
            if(section->data.size() - 0x7FF <= symbolTable[label]->offset) { // pc relative can be performed
                binary = serialize(JMP, cond, 15, r1, r2, section->data.size() - symbolTable[label]->offset); // jmp pc - ofset
                write_binary(section, binary);
                return 4;
            }
    }
    // relocation must be done, use jmp trick
    binary = serialize(JMP, 0x8+cond, 15, r1, r2, 0x4);
    write_binary(section, binary);
    binary = serialize(JMP, 0, 15, 0, 0, 0x4);
    write_binary(section, binary);
    section->reloc_table[label].push_back(section->data.size());
    write_binary(section, 0x0); // fill the memory for relocation with zeros 
    return 12;
}

void JmpInstruction::execute(CPU* cpu) {
    uint32 newPc = r1 + disp;
    
    switch(cond) {
        case (ALWAYS): break;
        case (BGT) : {
            if(r2 <= r3) return;
            break;
        }
        case (BEQ) : {
            if(r2 != r3) return;
            break;
        }
        case (BNE) : {
            if(r2 == r3) return;
            break;
        }
    }

    if(mod > 0xf) {
        newPc = cpu->readMem(newPc);
    }
    cpu->setRegister(PC, newPc);
}