#include "iret.hpp"
#include "load.hpp"

Instruction* IretInstruction::parsedInstruction(const std::string& instr, int reg1, int reg2, uint32 immediate, const std::string& label, int type) {
    return new IretInstruction();
}

Instruction* IretInstruction::binaryInstruction(int mod, int reg1, int reg2, int reg3, int disp) {
    return new IretInstruction();
}

int IretInstruction::writeSectionData(Section* section, std::unordered_map<std::string, Symbol*>& symbolTable) {
    uint32 binary = serialize(LOAD, 0x6, (short)STATUS, 14, 0, 4);
    write_binary(section, binary);
    binary = serialize(LOAD, 0x3, 15, 14, 0, 8);
    write_binary(section, binary);
    return 8;
}

void IretInstruction::execute(CPU* cpu) {
    return;
}