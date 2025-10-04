#ifndef COMMON_TYPES_HPP
#define COMMON_TYPES_HPP

#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>
#include <stdint.h>
#include <iomanip>
#include <utility>

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
    Symbol(const std::string& section, uint32 offset, bool global = false, bool defined = false) : section(section), offset(offset), global(global), defined(defined) {}
    ~Symbol() = default;

    std::string section;
    uint32 offset;
    bool global;
    bool defined;
};

class Section {
public:
    Section(uint32 addr, const std::string& name) : addr(addr), name(name) { reloc_table = {}; data = {}; }
    ~Section() = default;

    uint32 addr;
    std::string name;
    std::unordered_map<std::string, std::vector<uint32>> reloc_table;
    std::vector<uchar> data;
};

class EquExpression {
public:
    EquExpression(uint32 type, const std::string& symbol, uint32 value, const std::string& operand1, const std::string& operand2, uint32 operation)
        : type(type), symbol(symbol), value(value), operand1(operand1), operand2(operand2), operation(operation) {}
    ~EquExpression() = default;

    uint32 type;
    std::string symbol;
    uint32 value;
    std::string operand1;
    std::string operand2;
    uint32 operation;
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

enum LoadStoreOC {
    REGULAR = 0,
    STACK,
    CSR
};

#endif
