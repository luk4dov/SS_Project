#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include "types.hpp"
#include <exception>

class LocalSymbolNotDefined  : public std::exception {
public:
    LocalSymbolNotDefined(std::string symbolName, std::string fileName) {
        std::cerr << "Error: " << fileName << ": " << "Local symbol " << symbolName << " not defined.\n";
    }
};

class SymbolRedefinition : public std::exception {
public:
    SymbolRedefinition(std::string symbolName, std::string fileName, int line) {
        std::cerr << "Error: " << fileName << ": line " << line << ": " << "Redefinition of symbol: " << symbolName << ".\n";
    }
};

class SymbolMultipleDefinition : public std::exception {
public:
    SymbolMultipleDefinition(std::string symbolName) {
        std::cerr << "Error: " << "Multiple definition of symbol: " << symbolName << ".\n";
    }
};

class SymbolNotDefined : public std::exception {
public:
    SymbolNotDefined(std::string symbolName) {
        std::cerr << "Error: symbol " << symbolName << " is not defined\n";
    }
};

class SectionNotFound : public std::exception {
public:
    SectionNotFound(std::string sectionName) {
        std::cerr << "Error: section " << sectionName << " not found in section table\n";
    }
};

class SectionOverlap : public std::exception {
public:
    SectionOverlap(std::string sectionName, std::string otherSectionName) {
        std::cerr << "Error: sections " << sectionName << " and " << otherSectionName << " overlap\n";
    }
};


#endif