#include "instruction.hpp"
#include "instructions/halt.hpp"
#include "instructions/int.hpp"
#include "instructions/iret.hpp"
#include "instructions/call.hpp"
#include "instructions/jmp.hpp"
#include "instructions/xchg.hpp"
#include "instructions/load.hpp"
#include "instructions/store.hpp"
#include "instructions/arith.hpp"
#include "instructions/logic.hpp"
#include "instructions/shift.hpp"

using InstructionFactory = Instruction* (*) (const std::string&, int, int, uint32, const std::string&, int);

std::unordered_map<std::string, InstructionFactory> Instruction::instructions = {
    {"halt",  HaltInstruction::createInstruction},
    {"int",   IntInstruction::createInstruction},
    {"iret",  IretInstruction::createInstruction},
    {"call",  CallInstruction::createInstruction},
    {"ret",   LoadInstruction::createInstruction},
    {"jmp",   JmpInstruction::createInstruction},
    {"beq",   JmpInstruction::createInstruction},
    {"bne",   JmpInstruction::createInstruction},
    {"bgt",   JmpInstruction::createInstruction},
    {"push",  StoreInstruction::createInstruction},
    {"pop",   LoadInstruction::createInstruction},
    {"xchg",  XchgInstruction::createInstruction}, 
    {"add",   ArithInstruction::createInstruction},
    {"sub",   ArithInstruction::createInstruction},
    {"mul",   ArithInstruction::createInstruction},
    {"div",   ArithInstruction::createInstruction},
    {"not",   LogicInstruction::createInstruction},
    {"and",   LogicInstruction::createInstruction},
    {"or",    LogicInstruction::createInstruction},
    {"xor",   LogicInstruction::createInstruction},
    {"shl",   ShiftInstruction::createInstruction},
    {"shr",   ShiftInstruction::createInstruction},
    {"ld",    LoadInstruction::createInstruction},
    {"st",    StoreInstruction::createInstruction},
    {"csrrd", LoadInstruction::createInstruction},
    {"csrwr", StoreInstruction::createInstruction}
};

Instruction* Instruction::pickInstruction(const std::string& name, int r1, int r2, uint32 imm, const std::string& label, int type) {
    return instructions[name](name, r1, r2, imm, label, type);
}

uint32 Instruction::serialize(OperationCode op, short mn, short ra, short rb, short rc, short disp) {
    return (((op & 0xf) << 28) | 
            ((mn & 0xf) << 24) | 
            ((ra & 0xf) << 20) | 
            ((rb & 0xf) << 16) | 
            ((rc & 0xf) << 12) | 
            (disp & 0xfff));
}

void Instruction::write_binary(Section* section, uint32 binary_data) {
    section->data.push_back(binary_data >> 24);
    section->data.push_back((binary_data >> 16) & 0xff);
    section->data.push_back((binary_data >> 8) & 0xff);
    section->data.push_back(binary_data & 0xff);
}