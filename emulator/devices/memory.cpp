#include "memory.hpp"

Memory::Memory(const char* fileName) {
    BinaryRW* rw = new BinaryRW();
    mem = {};
    rw -> readHex(std::string(fileName), mem);
    delete rw;
}

int Memory::read(uint32 address) {
    uint32 block = address >> 12;
    uint32 word = address & 0xfff;

    if (mem.find(block) == mem.end()) {
        mem[block] = std::vector<uchar>(1 << 12, 0);
        return 0;
    }
    
    return static_cast<int>(mem[block][word] | (mem[block][word + 1] << 8) | (mem[block][word + 2] << 16) | (mem[block][word + 3] << 24));
}

void Memory::write(uint32 address, int value) {
    uint32 block = address >> 12;
    uint32 word = address & 0xfff;

    if(mem.find(block) == mem.end()) {
        mem[block] = std::vector<uchar>(1 << 12, 0);
    }
    
    // little endian
    mem[block][word] = value & 0xff;
    mem[block][word + 1] = (value >> 8) & 0xff;
    mem[block][word + 2] = (value >> 16) & 0xff;
    mem[block][word + 3] = (value >> 24) & 0xff;
}
