#include "instruction.hpp"
#include "instructions/halt.hpp"
#include "instructions/int.hpp"
#include "instructions/iret.hpp"
#include "instructions/call.hpp"
#include "instructions/ret.hpp"
#include "instructions/jmp.hpp"
#include "instructions/beq.hpp"
#include "instructions/bne.hpp"
#include "instructions/bgt.hpp"
#include "instructions/push.hpp"
#include "instructions/pop.hpp"
#include "instructions/xchg.hpp"
#include "instructions/add.hpp"
#include "instructions/sub.hpp"
#include "instructions/mul.hpp"
#include "instructions/div.hpp"
#include "instructions/not.hpp"
#include "instructions/and.hpp"
#include "instructions/or.hpp"
#include "instructions/xor.hpp"
#include "instructions/shl.hpp"
#include "instructions/shr.hpp"
#include "instructions/load.hpp"
#include "instructions/store.hpp"
#include "instructions/csrrd.hpp"
#include "instructions/csrwr.hpp"

#include <iostream>
#include <iomanip>

using InstructionFactory = Instruction* (*) (short, short, uint32, std::string, Section*, std::unordered_map<std::string, Symbol*>&, short);

std::unordered_map<std::string, InstructionFactory> Instruction::instructions = {
    {"halt",  HaltInstruction::createInstruction},
    {"int",   IntInstruction::createInstruction},
    {"iret",  IretInstruction::createInstruction},
    {"call",  CallInstruction::createInstruction},
    {"ret",   RetInstruction::createInstruction},
    {"jmp",   JmpInstruction::createInstruction},
    {"beq",   BeqInstruction::createInstruction},
    {"bne",   BneInstruction::createInstruction},
    {"bgt",   BgtInstruction::createInstruction},
    {"push",  PushInstruction::createInstruction},
    {"pop",   PopInstruction::createInstruction},
    {"xchg",  XchgInstruction::createInstruction},
    {"add",   AddInstruction::createInstruction},
    {"sub",   SubInstruction::createInstruction},
    {"mul",   MulInstruction::createInstruction},
    {"div",   DivInstruction::createInstruction},
    {"not",   NotInstruction::createInstruction},
    {"and",   AndInstruction::createInstruction},
    {"or",    OrInstruction::createInstruction},
    {"xor",   XorInstruction::createInstruction},
    {"shl",   ShlInstruction::createInstruction},
    {"shr",   ShrInstruction::createInstruction},
    {"ld",    LoadInstruction::createInstruction},
    {"st",    StoreInstruction::createInstruction},
    {"csrrd", CsrrdInstruction::createInstruction},
    {"csrwr", CsrwrInstruction::createInstruction}
};

Instruction* Instruction::pickInstruction(std::string name, short r1, short r2, uint32 imm, std::string label, 
                                          Section* section, std::unordered_map<std::string, Symbol*>& symbolTable, short type) {
    return instructions[name](r1, r2, imm, label, section, symbolTable, type);
}

uint32 Instruction::serialize(OperationCode op, short mn, short ra, short rb, short rc, short disp) {
    return (((op & 0xf) << 28) | ((mn & 0xf) << 24) | ((ra & 0xf) << 20) | ((rb & 0xf) << 16) | ((rc & 0xf) << 12) | (disp & 0xfff));
}

void Instruction::write_binary(Section* section, uint32 binary_data) {
    section->data.push_back(binary_data >> 24);
    section->data.push_back((binary_data >> 16) & 0xff);
    section->data.push_back((binary_data >> 8) & 0xff);
    section->data.push_back(binary_data & 0xff);
    std::cout << std::hex << std::setw(2) << (binary_data >> 24) << " "
                          << std::setw(2) << ((binary_data >> 16) & 0xff) << " "
                          << std::setw(2) << ((binary_data >> 8) & 0xff) << " "
                          << std::setw(2) << (binary_data & 0xff) << '\n';
}