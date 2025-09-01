#include "assembler.hpp"

static Assembler* assembler;

void selectInstruction(const char* name, int reg1, int reg2, uint32 immediate, const char* label, int type) {
    assembler->selectInstruction(std::string(name), reg1, reg2, immediate, std::string(label), type);
}

void labelDefinition(const char* name) {
    assembler->labelDefinition(std::string(name));
}

void selectDirective(const char* dir, const char* nm, uint32 immediate) {
    assembler->selectDirective(std::string(dir), std::string(nm), immediate);
}

int main(int argc, char* argv[]) {
    if(argc < 2 || argc > 4 || argc == 3) {
        std::cerr << "Error: Invalid arguments\n";
        return -1;
    }
    
    const char* inputFile = argv[argc-1];
    const char* outputFile;

    if(argc > 2) 
        outputFile = argv[2];
    else
        outputFile = "./izlazniFajl.o";

    assembler = new Assembler(inputFile, outputFile);
    
    int ret = assembler->assemble();
    
    delete assembler;

    return ret;
}