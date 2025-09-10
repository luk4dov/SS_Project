#include "emulator.hpp"

Emulator::Emulator(const char* fileName) {
    memory = new Memory(fileName);
    cpu = new CPU(memory);
}

Emulator::~Emulator() {
    delete cpu;
    delete memory;
}

int Emulator::emulate() {
    while(!cpu->getHalt()) {    
        cpu->executeInstruction();
    }

    cpu->printContext();
    return 0;
}