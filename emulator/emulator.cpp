#include "emulator.hpp"

Emulator::Emulator(const char* fileName) {
    memory = new Memory(fileName);
    cpu = new CPU(memory);
    terminal = new Terminal(cpu);
    timer = new Timer(cpu);
}

Emulator::~Emulator() {
    delete terminal;
    delete timer;
    delete cpu;
    delete memory;
}

int Emulator::emulate() {
    while(!cpu -> getHalt()) {
        cpu -> executeInstruction();

        terminal -> check();

        timer -> tick();
    }

    cpu->printContext();
    return 0;
}