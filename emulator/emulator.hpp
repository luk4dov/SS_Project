#ifndef EMULATOR_HPP
#define EMULATOR_HPP

#include "./devices/cpu.hpp"
#include "./devices/memory.hpp"
#include "./devices/terminal.hpp"
#include "./devices/timer.hpp"

class Emulator {
public:
    Emulator(const char*);
    ~Emulator();

    int emulate();

private:
    CPU* cpu;
    Memory* memory;
    Terminal* terminal;
    Timer* timer;

};


#endif
