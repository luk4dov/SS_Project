#ifndef EMULATOR_HPP
#define EMULATOR_HPP

#include "../common/types.hpp"

class Emulator {
public:
    Emulator(const char*);

private:
    std::unordered_map<uint32, std::vector<uchar>> mem;

    // CPU
    // terminal
    // timer
    
};


#endif