#include "store.hpp"


StoreInstruction::StoreInstruction(int r1, int r2, uint32 immediate, const std::string& label, int type, LoadStoreOC op) :
            Instruction("store"), r1(r1), r2(r2), immediate(immediate), label(label), type(type), op(op) {};

StoreInstruction::StoreInstruction(int mod, int r1, int r2, int r3, int disp) : Instruction("store"), r1(r1), r2(r2), r3(r3), disp(disp), mod(mod) 
{
    if(mod == 1) op = STACK;
    else op = REGULAR;
};

Instruction* StoreInstruction::parsedInstruction(const std::string& instr, int r1, int r2, uint32 immediate, const std::string& label, int type) {
    if(instr == "push") {
        return new StoreInstruction(r1, 14, 0, label, type, STACK);
    }
    return new StoreInstruction(r1, r2, immediate, label, type, REGULAR);
}

Instruction* StoreInstruction::binaryInstruction(int mod, int r1, int r2, int r3, int disp) {
    return new StoreInstruction(mod, r1, r2, r3, disp);
}

int StoreInstruction::writeSectionData(Section* section, std::unordered_map<std::string, Symbol*>& symbolTable) {
    if(op == STACK) { // push
        uint32 binary = serialize(STORE, 1, r2, 0, r1, -4); // ra, rb, rc, disp
        write_binary(section, binary);
        return 4;
    }
    
    switch(type) {
        case 0: { // STORE reg, imm -> mem[imm] <= reg
            if((int)immediate >= MIN_VAL && (int)immediate <= MAX_VAL) {
                uint32 binary = serialize(STORE, 0, 0, 0, r1, immediate); // mem[imm] <= rs
                write_binary(section, binary);
                return 4;
            }
            uint32 binary = serialize(STORE, 2, 15, 0, r1, 0x4);
            write_binary(section, binary);
            binary = serialize(JMP, 0, 15, 0, 0, 0x4);
            write_binary(section, binary);
            write_binary(section, immediate);
            return 12;
        }
        case 1: { // STORE reg, sym -> mem[sym] <= reg
            if(symbolTable.find(label) == symbolTable.end()) {
                symbolTable[label] = new Symbol("UND", 0);
            }
            else if(symbolTable[label]->defined && section->data.size() - symbolTable[label]->offset < MAX_VAL) { // can be pc relative
                uint32 binary = serialize(STORE, 0x0, 15, 0, r1, symbolTable[label]->offset - section->data.size()); // mem[pc + o - offset] <= rs
                write_binary(section, binary);
                return 4;
            }
            // relocation must be done
            uint32 binary = serialize(STORE, 0x2, 15, 0, r1, 0x4);
            write_binary(section, binary);
            binary = serialize(JMP, 0, 15, 0, 0, 4);
            write_binary(section, binary);
            section->reloc_table[label].push_back(section->data.size());
            write_binary(section, 0x0);
            return 12;
        }
        case 2: { // STORE reg, [reg1] -> mem[reg] <= reg
            uint32 binary = serialize(STORE, 0, r2, 0, r1, 0);
            write_binary(section, binary);
            return 4;
        }
        case 3: { // STORE reg, [reg1+imm] -> mem[reg1+imm], reg
            if((int)immediate < MIN_VAL || (int)immediate > MAX_VAL) return -1;
            uint32 binary = serialize(STORE, 0, r2, 0, r1, immediate);
            write_binary(section, binary);
            return 4;
        }
    }
    return 0;
}

void StoreInstruction::execute(CPU* cpu) {
    switch(op) {
        case STACK: {
            uint32 sp = cpu->getRegister(SP);
            sp += disp;
            cpu->writeMem(sp, r3);
            cpu->setRegister(SP, sp);
            break;
        }
        case CSR: {
            return;
        }
        case REGULAR: {
            uint32 address = cpu->getRegister((REGS)r1) + cpu->getRegister((REGS)r2) + immediate;
            if(mod == 2) {
                address = static_cast<uint32>(cpu->readMem(address));
            }
            cpu->writeMem(address, r3);
        }
    }
}