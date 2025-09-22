#include "emulator.hpp"

Emulator::Emulator(const char* fileName) {
    memory = new Memory(fileName);
    cpu = new CPU(memory);
    terminal = new Terminal(cpu);
}

Emulator::~Emulator() {
    delete terminal;
    delete cpu;
    delete memory;
}

int Emulator::emulate() {
    while(!cpu -> getHalt()) {
        cpu -> executeInstruction();

        terminal -> check();
    }

    cpu->printContext();
    return 0;
}