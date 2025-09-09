#include "emulator.hpp"

static Emulator* emulator;

int main(int argc, char* argv[]) {
    if(argc != 2) {
        std::cerr << "Error: Invalid arguments\n";
        return -1;
    }
    
    emulator = new Emulator(argv[1]);

    int ret = emulator->emulate();

    delete emulator;
    
    return ret;
}