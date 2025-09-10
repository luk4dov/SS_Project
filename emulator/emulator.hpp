#ifndef EMULATOR_HPP
#define EMULATOR_HPP

#include "../common/types.hpp"
#include "./devices/cpu.hpp"
#include "./devices/memory.hpp"

class Emulator {
public:
    Emulator(const char*);
    ~Emulator();

    int emulate();

private:

    std::unordered_map<uint32, std::vector<uchar>> mem;

    CPU* cpu;
    Memory* memory;
    
};


#endif