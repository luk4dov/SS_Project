#ifndef MEMORY_HPP
#define MEMORY_HPP

#include "../../common/types.hpp"
#include "../../common/binaryrw.hpp"

class Memory {
public:
    Memory(const char*);
    ~Memory();

    int read(uint32 address);
    void write(uint32 address, uint32 value);

private:
    std::unordered_map<uint32, std::vector<uchar>> mem;
};

#endif