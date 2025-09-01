#include "shift.hpp"

Instruction* ShiftInstruction::createInstruction(const std::string& instr, int r1, int r2, uint32 imm, const std::string& label, int type) {
    ShiftOC mod = LEFT;

    if(instr == "shr") {
        mod = RIGHT;
    }

    return new ShiftInstruction(r2, r2, r1, mod);
}

int ShiftInstruction::writeSectionData(Section* section, std::unordered_map<std::string, Symbol*>& symbolTable) {
    uint32 binary = serialize(SHIFT, mod, r1, r2, r3, 0x0);
    write_binary(section, binary);
    return 4;
}
