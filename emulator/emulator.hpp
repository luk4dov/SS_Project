#ifndef EMULATOR_HPP
#define EMULATOR_HPP

#include "../common/types.hpp"
#include "./devices/cpu.hpp"
#include "./devices/memory.hpp"
#include "./devices/terminal.hpp"

class Emulator {
public:
    Emulator(const char*);
    ~Emulator();

    int emulate();

private:
    CPU* cpu;
    Memory* memory;
    Terminal* terminal;
    
};


#endif