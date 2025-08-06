#ifndef INSTRUCTION_H
#define INSTRUCTION_H

typedef uint uint32;

typedef struct {
    const char* name;
    short reg1;
    short reg2;
    uint32 imm;
    char* label;
} instr_data;

#endif