#include <iostream>
#include <string.h>
#include "../inc/instruction.hpp"

void selectInstructionWrap(std::string name, short reg1 = -1, short reg2 = -1, uint32 immediate = -1, std::string label = "") {
    if(name == "halt") {std::cout << "assembler.cpp:halt:\n";}
    else if (name == "ret") { std::cout << "assembler.cpp:ret:\n"; }
    else if (name == "call") { std::cout << "assembler.cpp:call: " << label << '\n'; }
    else if (name == "jmp") { std::cout << "assembler.cpp:jmp:" << label << '\n'; }
    else if (name == "beq") { std::cout << "assembler.cpp:beq:" << reg1 << ", " << reg2 << ", "<< label << '\n';}
}

extern "C" void selectInstruction(char* name, short reg1, short reg2, uint32 immediate, char* label)
{
    selectInstructionWrap(std::string(name), reg1, reg2, immediate, std::string(label));
}

