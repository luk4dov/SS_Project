#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include "types.hpp"
#include <exception>


class LiteralNotIn12Bits : public std::exception {
public:
    LiteralNotIn12Bits(uint32 literal) {
        std::cerr << "Error: literal " << literal << " is not in 12 bits.\n";
    }
};

class SymbolNotLiteralNotIn12Bits : public std::exception {
public:
    SymbolNotLiteralNotIn12Bits(const std::string& symbol) {
        std::cerr << "Error: symbol " << symbol << " is not in 12 bits.\n";
    }
};

class LocalSymbolNotDefined  : public std::exception {
public:
    LocalSymbolNotDefined(const std::string& symbolName, const std::string& fileName) {
        std::cerr << "Error: " << fileName << ": " << "Local symbol " << symbolName << " not defined.\n";
    }
};

class SymbolRedefinition : public std::exception {
public:
    SymbolRedefinition(const std::string& symbolName, const std::string& fileName, int line) {
        std::cerr << "Error: " << fileName << ": line " << line << ": " << "Redefinition of symbol: " << symbolName << ".\n";
    }
};

class InitializationWithNonEquSymbol : public std::exception {
public:
    InitializationWithNonEquSymbol(const std::string& symbolName, const std::string& fileName, int line) {
        std::cerr << "Error: " << fileName << ": line " << line << ": " << "Initialization of symbol: " << symbolName << " with non .equ symbol.\n";
    }
};

class CircularDependencyInEqu : public std::exception {
public:
    CircularDependencyInEqu(const std::string& fileName) {
        std::cerr << "Error: " << fileName << ": " << "Circular dependency in .equ directives.\n";
    }
};

class SymbolMultipleDefinition : public std::exception {
public:
    SymbolMultipleDefinition(const std::string& symbolName) {
        std::cerr << "Error: " << "Multiple definition of symbol: " << symbolName << ".\n";
    }
};

class SymbolNotDefined : public std::exception {
public:
    SymbolNotDefined(const std::string& symbolName) {
        std::cerr << "Error: symbol " << symbolName << " is not defined\n";
    }
};

class SectionNotFound : public std::exception {
public:
    SectionNotFound(const std::string& sectionName) {
        std::cerr << "Error: section " << sectionName << " not found in section table\n";
    }
};

class SectionOverlap : public std::exception {
public:
    SectionOverlap(const std::string& sectionName, const std::string& otherSectionName) {
        std::cerr << "Error: sections " << sectionName << " and " << otherSectionName << " overlap\n";
    }
};

class DivisionByZero : public std::exception {
public:
    DivisionByZero() {
        std::cerr << "Error: division by zero\n";
    }
};

class InvalidAddress : public std::exception {
public:
    InvalidAddress() {
        std::cerr << "Error: Invalid address\n";
    }
};

#endif
