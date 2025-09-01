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

using InstructionFormat = Instruction* (*) (const std::string&, int, int, uint32, const std::string&, int);

std::unordered_map<std::string, InstructionFormat> Instruction::parsedInstructions = {
    {"halt",  HaltInstruction::parsedInstruction},
    {"int",   IntInstruction::parsedInstruction},
    {"iret",  IretInstruction::parsedInstruction},
    {"call",  CallInstruction::parsedInstruction},
    {"ret",   LoadInstruction::parsedInstruction},
    {"jmp",   JmpInstruction::parsedInstruction},
    {"beq",   JmpInstruction::parsedInstruction},
    {"bne",   JmpInstruction::parsedInstruction},
    {"bgt",   JmpInstruction::parsedInstruction},
    {"push",  StoreInstruction::parsedInstruction},
    {"pop",   LoadInstruction::parsedInstruction},
    {"xchg",  XchgInstruction::parsedInstruction},
    {"add",   ArithInstruction::parsedInstruction},
    {"sub",   ArithInstruction::parsedInstruction},
    {"mul",   ArithInstruction::parsedInstruction},
    {"div",   ArithInstruction::parsedInstruction},
    {"not",   LogicInstruction::parsedInstruction},
    {"and",   LogicInstruction::parsedInstruction},
    {"or",    LogicInstruction::parsedInstruction},
    {"xor",   LogicInstruction::parsedInstruction},
    {"shl",   ShiftInstruction::parsedInstruction},
    {"shr",   ShiftInstruction::parsedInstruction},
    {"ld",    LoadInstruction::parsedInstruction},
    {"st",    StoreInstruction::parsedInstruction},
    {"csrrd", LoadInstruction::parsedInstruction},
    {"csrwr", StoreInstruction::parsedInstruction}
};

std::unordered_map<OperationCode, Instruction* (*) (int,int,int,int,int)> Instruction::binaryInstructions = {
    {HALT, HaltInstruction::binaryInstruction},
    {INT, IntInstruction::binaryInstruction},
    {CALL, CallInstruction::binaryInstruction},
    {JMP, JmpInstruction::binaryInstruction},
    {ASWAP, XchgInstruction::binaryInstruction},
    {ARITH, ArithInstruction::binaryInstruction},
    {LOGIC, LogicInstruction::binaryInstruction},
    {SHIFT, ShiftInstruction::binaryInstruction},
    {LOAD, LoadInstruction::binaryInstruction},
    {STORE, StoreInstruction::binaryInstruction}
};

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