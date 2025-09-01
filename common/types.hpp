#ifndef COMMON_TYPES_HPP
#define COMMON_TYPES_HPP

#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>
#include <stdint.h>
#include <iomanip>

typedef uint32_t uint32;
typedef uint8_t uchar;

#define MAX_VAL 2047
#define MIN_VAL -2048

enum {
    CAUSE = 0,
    HANDLER,
    STATUS
} typedef CSRREG;

enum {
    ALWAYS = 0,
    BEQ,
    BNE,
    BGT
} typedef JMPCondition;


class Symbol {
public:
    Symbol(std::string section, uint32 offset, bool global = false, bool defined = false) : section(section), offset(offset), global(global), defined(defined) {}
    ~Symbol() = default;

    std::string section;
    uint32 offset;
    bool global;
    bool defined;
};

class Section {
public:
    Section(uint32 addr, uint32 offset, std::string name) : addr(addr), name(name) { reloc_table = {}; data = {}; }
    ~Section() = default;

    uint32 addr;
    std::string name;
    std::unordered_map<std::string, std::vector<uint32>> reloc_table;
    std::vector<uchar> data;
};

enum {
    HALT = 0,
    INT,
    CALL,
    JMP,
    ASWAP,
    ARITH,
    LOGIC,
    SHIFT,
    STORE,
    LOAD
} typedef OperationCode;

enum {
    ADD = 0,
    SUB,
    MUL,
    DIV
} typedef ArithOC;

enum {
    NOT = 0,
    AND,
    OR,
    XOR
} typedef LogicOC;

enum {
    LEFT = 0,
    RIGHT
} typedef ShiftOC;

enum {
    REGULAR = 0,
    STACK = 1,
    CSR = 2
} typedef LoadStoreOC;

#endif